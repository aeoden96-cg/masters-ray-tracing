//
// Created by mateo on 21.8.22.
//

#ifndef PROJECT_MATERIAL_H
#define PROJECT_MATERIAL_H

#include "./../utils/utils.h"
#include "./../objects/hittable.h"
#include "texture.h"

struct hit_record;

class material {
public:

    /**
     * @brief Vraća boju koju objekt zrači (npr. boja svjetlosti)
     * @param u koordinata
     * @param v koordinata
     * @param p točka na površini objekta
     * @return boja koju objekt zrači
     */
    virtual color emitted(double u, double v, const point3& p) const {
        return color(0,0,0);
    }

    /**
     * @brief Određuje kako će se zraka odbiti od materijala. Ako se zraka ne odbija, vraća false (kao na primjer svjetlo)
     * @param r_in ulazna zraka
     * @param rec informacije o udarcu
     * @param attenuation koeficijent atenuacije (prigušenja)
     * @param scattered odbijena zraka. Argument se vraća kao referenca
     * @return true ako se zraka odbija, tada se u scattered vraća odbijena zraka
     * @return false ako se zraka ne odbija, tada se scattered ne mijenja
     */
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;

    [[nodiscard]] virtual color emitted() const {
        return color(0,0,0);
    }
};


/**
 * @brief Materijal koji se sastoji od jedne boje.Zraka se odbija u slučajnom smjeru.
 * @param albedo boja materijala
 */
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in,
                 const hit_record& rec,
                 color& attenuation,
                 ray& scattered
                 ) const override
                 {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};



/**
 * @brief Materijal metala. Zraka se odbija u slučajnom smjeru, ali jako blizu vektora normale.
 * @param albedo boja materijala
 * @param fuzz koeficijent glatkosti metala, vrijednosti između 0 i 1
 */
class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
            ) const override {

        glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    float fuzz;
};


/**
 * @brief Materijal prozirnosti. Zraka može proći kroz materijal, ali se neće odbiti, ili se može odbiti.
 * @param index_of_refraction indeks loma materijala
 *
 */
class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
            ) const override {
            attenuation = color(1.0, 1.0, 1.0);
            float refraction_ratio = rec.front_face ? (1.0/ir) : ir;
            glm::vec3 unit_direction = glm::normalize(r_in.direction());

            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            glm::vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction);
            return true;
        }

    public:
        double ir; // Index of Refraction

    private:
        static double reflectance(float cosine, float ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

class diffuse_light : public material  {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};




#endif //PROJECT_MATERIAL_H
