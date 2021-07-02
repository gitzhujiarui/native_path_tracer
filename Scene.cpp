#include "Scene.hpp"

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const {
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const {
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum) {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object *> &objects,
        float &tNear, uint32_t &index, Object **hitObject) {
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

Vector3f Scene::castRay(const Ray &ray, int depth) const {
    auto inter = intersect(ray);
    if (!inter.happened) {
        return Vector3f();
    }
    Material *m = inter.m;
    auto p = inter.coords;
    auto N = inter.normal;
    // auto wo = ray.direction_inv;
    auto wo = -ray.direction;

    if (inter.obj -> hasEmit()) return inter.emit;

    if (m->getType() == DIFFUSE) {
        Vector3f L_dir;
        Vector3f L_indir;
        Intersection light_inter;
        float light_pdf = 0.0;

        sampleLight(light_inter, light_pdf);
        auto L_i = light_inter.emit;
        auto x_prime = light_inter.coords;
        auto N_prime = light_inter.normal;
        auto w = -(x_prime - p);           
        auto w_normalized = w.normalized();

        float light_area = 1.0 / light_pdf;
        float cos_theta = dotProduct(N.normalized(), -w.normalized());
        float cos_theta_prime = dotProduct(w_normalized, N_prime.normalized());

        float pdf_light_w = w.norm() * w.norm() / (light_area * cos_theta_prime);

        // shoot a ray from p to x_prime
        // if the ray is not block in the middle
        auto obj_to_light_ray = Ray(p, -w);
        auto light_blocked = intersect(obj_to_light_ray);
        if (light_blocked.happened && light_blocked.obj -> hasEmit()) {
            L_dir = L_i * inter.m -> eval(wo, -w, N) * cos_theta / pdf_light_w;
        }

        // contribution fron other reflectors
        float p_RR = get_random_float();
        if (depth >= 5) {
            if (p_RR < RussianRoulette) {
                // randomly sample the hemisphere toward w_i(pdf_brdf)
                // Trace a ray r(p, w_i)
                Vector3f obj_to_obj_normalized = inter.m -> sample(wo, N).normalized();
                float pdf_brdf = inter.m -> pdf(wo, obj_to_obj_normalized, N);
                Ray obj_to_obj_ray = Ray(p, obj_to_obj_normalized);
                auto obj_inter = intersect(obj_to_obj_ray);
                if (obj_inter.happened && !(obj_inter.m -> hasEmission())) {
                    L_indir = castRay(obj_to_obj_ray, depth++) * inter.m -> eval(wo, obj_to_obj_normalized, N) *
                                dotProduct(N.normalized(), obj_to_obj_normalized) / pdf_brdf / RussianRoulette;
                }   
            }
        } else {
                Vector3f obj_to_obj_normalized = inter.m -> sample(wo, N).normalized();
                float pdf_brdf = inter.m -> pdf(wo, obj_to_obj_normalized, N);
                Ray obj_to_obj_ray = Ray(p, obj_to_obj_normalized);
                auto obj_inter = intersect(obj_to_obj_ray);
                if (obj_inter.happened && !(obj_inter.m -> hasEmission())) {
                    L_indir = castRay(obj_to_obj_ray, depth++) * inter.m -> eval(wo, obj_to_obj_normalized, N) *
                                dotProduct(N.normalized(), obj_to_obj_normalized) / pdf_brdf;
                }   
        }
        auto result = L_dir + L_indir;
        
        return result;
    } else if (m->getType() == SPECULAR) {
        // perfect SPECULAR
        // incidence angle = refleciton angle
        float kr;
        fresnel(ray.direction, N, m->ior, kr);
        Vector3f spectrum = Vector3f(0.0f);
        Vector3f refl_dir = reflect(ray.direction, N);
        Vector3f refl_ori;
        if (dotProduct(refl_dir, N) < 0) {
            refl_ori = p + N * EPSILON; 
        } else {
            refl_ori = p - N * EPSILON;
        }
        auto result = castRay(Ray(refl_ori, refl_dir),depth++) * kr;
        return result;
    } else {
        // GLASS
        // need to consider both reflection and refraction
        float eta = (wo.z > 0) ? (1.f / m->ior) : m->ior;
        float predicate = 1 - eta * eta * (1 - wo.z * wo.z);
        Vector3f refl_dir = normalize(reflect(ray.direction, N));
        Vector3f refra_dir = normalize(refract(ray.direction, N, m->ior));
        Vector3f spectrum = Vector3f(0.0f);
        Vector3f refl_ori;
        Vector3f refra_ori;
        if (dotProduct(refl_dir, N) < 0) refl_ori = p - N * EPSILON;
        else refl_ori = p + N * EPSILON;
        if (dotProduct(refra_dir, N) < 0) refra_ori = p - N * EPSILON;
        else refra_ori = p + N * EPSILON;
        Vector3f refl_part = castRay(Ray(refl_ori, refl_dir), depth++);
        Vector3f refra_part = castRay(Ray(refra_ori, refra_dir), depth++);
        float kr;
        fresnel(ray.direction, N, m->ior, kr);
        Vector3f result = refl_part * kr + refra_part * (1 - kr);
        return result;
    }
    return Vector3f(0.0);
}