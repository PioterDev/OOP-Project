#pragma once

#include <cstddef>

namespace Math {
    //The largest value of n so that n!≤ 2⁶⁴ - 1
    constexpr size_t maxN64BFact = 20;

    //π
    constexpr double PI = 3.141592653589793;
    //π (float)
    constexpr float PI_F = 3.1415927f;
    //π (long double)
    constexpr long double PI_L = 3.141592653589793238462643383279502884197169399375L;

    //1/π
    constexpr double INV_PI = 1.0 / 3.141592653589793;
    //1/π (float)
    constexpr float INV_PI_F = 1.0f / 3.1415927f;
    //1/π (long double)
    constexpr long double INV_PI_L = 1.0L / 3.141592653589793238462643383279502884197169399375L;


    //τ (2π)
    constexpr double TAU = 2.0 * PI;
    //τ (2π) (float)
    constexpr float TAU_F = 2.0f * PI_F;
    //τ (2π) (long double)
    constexpr long double TAU_L = 2.0L * PI_L;

    //1/τ (1/2π)
    constexpr double INV_TAU = 1.0 / TAU;
    //1/τ (1/2π) (float)
    constexpr float INV_TAU_F = 1.0f / TAU_F;
    //1/τ (1/2π) (long double)
    constexpr long double INV_TAU_L = 1.0L / TAU_L;


    //e
    constexpr double E = 2.718281828459045;
    //e (float)
    constexpr double E_F = 2.7182817f;
    //e (long double)
    constexpr double E_L = 2.718281828459045235360287471352662497757247093699959574966967627724076630353548L;

    //1/e
    constexpr double INV_E = 1.0 / E;
    //1/e (float)
    constexpr double INV_E_F = 1.0f / E_F;
    //1/e (long double)
    constexpr double INV_E_L = 1.0L / E_L;


    //√2
    constexpr double SQRT2 = 1.4142135623730951;
    //√2 (float)
    constexpr float SQRT2_F = 1.4142135f;
    //√2 (long double)
    constexpr long double SQRT2_L = 1.41421356237309504880168872420969807856967187537694807317667973799L;

    //1/√2
    constexpr double INV_SQRT2 = 1.0 / SQRT2;
    //1/√2 (float)
    constexpr float INV_SQRT2_F = 1.0f / SQRT2_F;
    //1/√2 (long double)
    constexpr long double INV_SQRT2_L = 1.0L / SQRT2_L;

    //√3
    constexpr double SQRT3 = 1.7320508075688772;
    //√3 (float)
    constexpr float SQRT3_F = 1.7320508f;
    //√3 (long double)
    constexpr long double SQRT3_L = 1.732050807568877293527446341505872366942805253810380628055806L;

    //1/√3
    constexpr double INV_SQRT3 = 1.0 / SQRT3;
    //1/√3 (float)
    constexpr float INV_SQRT3_F = 1.0f / SQRT3_F;
    //1/√3 (long double)
    constexpr long double INV_SQRT3_L = 1.0L / SQRT3_L;

    //√5
    constexpr double SQRT5 = 2.23606797749979;
    //√5 (float)
    constexpr float SQRT5_F = 2.236068f;
    //√5 (long double)
    constexpr long double SQRT5_L = 2.23606797749978969640917366873127623544061835961152572427089L;

    //1/√5
    constexpr double INV_SQRT5 = 1.0 / SQRT5;
    //1/√5 (float)
    constexpr float INV_SQRT5_F = 1.0f / SQRT5_F;
    //1/√5 (long double)
    constexpr long double INV_SQRT5_L = 1.0L / SQRT5_L;

    //∛2
    constexpr double CBRT2 = 1.2599210498948732;
    //∛2 (float)
    constexpr float CBRT2_F = 1.2599211f;
    //∛2 (long double)
    constexpr long double CBRT2_L = 1.25992104989487316476721060727822835057025146470150798008197511215529967651L;

    //1/∛2
    constexpr double INV_CBRT2 = 1.0 / CBRT2;
    //1/∛2 (float)
    constexpr float INV_CBRT2_F = 1.0f / CBRT2_F;
    //1/∛2 (long double)
    constexpr long double INV_CBRT2_L = 1.0L / CBRT2_L;

    //∛3
    constexpr double CBRT3 = 1.4422495703074083;
    //∛3 (float)
    constexpr float CBRT3_F = 1.4422495f;
    //∛3 (long double)
    constexpr long double CBRT3_L = 1.442249570307408382321638310780109588391869253499350577546416194541687596829997L;

    //1/∛3
    constexpr double INV_CBRT3 = 1.0 / CBRT3;
    //1/∛3 (float)
    constexpr float INV_CBRT3_F = 1.0f / CBRT3_F;
    //1/∛3 (long double)
    constexpr long double INV_CBRT3_L = 1.0L / CBRT3_L;


    //ln 2
    constexpr double LN2 = 0.6931471805599453;
    //ln 2 (float)
    constexpr float LN2_F = 0.6931472f;
    //ln 2 (long double)
    constexpr long double LN2_L = 0.6931471805599453094172321214581765680755001343602552541206800094933936219696947L;

    //1/ln 2
    constexpr double INV_LN2 = 1.0 / LN2;
    //1/ln 2 (float)
    constexpr float INV_LN2_F = 1.0f / LN2_F;
    //1/ln 2 (long double)
    constexpr long double INV_LN2_L = 1.0L / LN2_L;


    //φ (Golden ratio)
    constexpr double PHI = 1.618033988749895;
    //φ (Golden ratio) (float)
    constexpr float PHI_F = 1.618034f;
    //φ (Golden ratio) (long double)
    constexpr long double PHI_L = 1.61803398874989484820458683436563811772030917980576286213544L;

    //1/φ (1/Golden ratio)
    constexpr double INV_PHI = 1.0 / PHI;
    //1/φ (1/Golden ratio) (float)
    constexpr float INV_PHI_F = 1.0f / PHI_F;
    //1/φ (1/Golden ratio) (long double)
    constexpr long double INV_PHI_L = 1.0L / PHI_L;


    template<typename T> struct is_float {
        static const bool value = false;
    };

    template<> struct is_float<float> {
        static const bool value = true;
    };

    template<> struct is_float<double> {
        static const bool value = true;
    };

    template<> struct is_float<long double> {
        static const bool value = true;
    };

    /**
     * @brief A table of `N!`, where 0 <= `N` <= 20.
     * The upper limit of 20 is because 21! > 2^64 and I
     * don't feel like using __int128.
     * 
     * Values are computed at compile-time, but
     * creating a stack instance (even a constexpr one)
     * results in a memcpy, you can verify this
     * by looking at the assembly generated.
     * 
     * @tparam N number of entries in the table
     */
    template<size_t N> class FactorialTable {
        public:
        size_t values[N+1];
        constexpr FactorialTable() : values() {
            static_assert(N <= 20);
            values[0] = 1;
            for(size_t i = 1; i <= N; i++) {
                values[i] = values[i - 1] * i;
            }
        }
    };

    /**
     * @brief A table of sine 
     * in [0, π/2) radians with `N` entries.
     * For example, if `N == 90`, the generated
     * table will contain sine values of
     * 0, 1, ..., 89 degrees.
     * 
     * Values are computed at compile-time, but
     * creating a stack instance (even a constexpr one)
     * results in a memcpy, you can verify this
     * by looking at the assembly generated.
     * 
     * @tparam N number of entries in the table
     */
    template<size_t N, typename T = double> class SinTable {
        public:
        T values[N];
        constexpr SinTable() : values() {
            static_assert(is_float<T>::value, "A sine table requires a floating-point type");
            constexpr FactorialTable<maxN64BFact> f;

            size_t i;
            T alpha;
            for(i = 0, alpha = (T)0.0; i < N; i++, alpha += (T)PI_L / ((T)2.0 * (T)N)) {
                //see Maclaurin series for sin(x)
                size_t j, k;
                T gamma = alpha * alpha, beta = alpha;
                for(j = 1, k = 0, beta = alpha; j <= maxN64BFact; j += 2, k++, beta *= gamma) {
                    if(k & 1) values[i] -= beta / (T)f.values[j];
                    else values[i] += beta / (T)f.values[j];
                }
            }
        }

        static constexpr T at(T angle) {
            constexpr SinTable<N, T> s;
            while(angle >= (T)360.0) { angle -= (T)360.0; }
            while(angle <= (T)-360.0) { angle += (T)360.0; }

            if(angle == (T)90.0) return (T)1.0;
            else if(angle == (T)270.0) return (T)-1.0;

            T alpha;
            if(angle <= (T)90.0) alpha = angle;
            else if(angle <= (T)180.0) alpha = (T)180.0 - angle;
            else if(angle <= (T)270.0) alpha = angle - (T)180.0;
            else alpha = (T)360.0 - angle;

            if(alpha == (T)0.0) return (T)0.0;
            

            T beta = s.values[(size_t)(alpha / (T)90.0 * (T)N)];
            if(angle <= (T)180.0) return beta;
            else return -beta;
        }
    };

    /**
     * @brief A table of cosine 
     * in [0, π/2] radians with `N` entries.
     * For example, if `N == 90`, the generated
     * table will contain sine values of
     * 0, 1, ..., 89 degrees.
     * 
     * Values are computed at compile-time, but
     * creating a stack instance (even a constexpr one)
     * results in a memcpy, you can verify this
     * by looking at the assembly generated.
     * 
     * @tparam N number of entries in the table
     */
    template<size_t N, typename T = double> class CosTable {
        public:
        T values[N];
        constexpr CosTable() : values() {
            static_assert(is_float<T>::value, "A cosine table requires a floating-point type");
            constexpr FactorialTable<maxN64BFact> f;

            size_t i;
            T alpha;
            for(i = 0, alpha = (T)0.0; i < N; i++, alpha += (T)PI_L / ((T)2.0 * (T)N)) {
                //see Maclaurin series for cos(x)
                values[i] = (T)1.0;
                size_t j, k;
                T gamma = alpha * alpha, beta;
                for(j = 2, k = 1, beta = gamma; j <= maxN64BFact; j += 2, k++, beta *= gamma) {
                    if(k & 1) values[i] -= beta / (T)f.values[j];
                    else values[i] += beta / (T)f.values[j];
                }
            }
        }

        static constexpr T at(T angle) {
            constexpr CosTable<N, T> s;
            while(angle >= (T)360.0) { angle -= (T)360.0; }
            while(angle <= (T)-360.0) { angle += (T)360.0; }

            if(angle == (T)0.0) return (T)1.0;
            else if(angle == (T)180.0) return (T)-1.0;

            T alpha;
            if(angle <= (T)90.0) alpha = angle;
            else if(angle <= (T)180.0) alpha = (T)180.0 - angle; //cos(180 - x) = -cos(x)
            else if(angle <= (T)270.0) alpha = angle - (T)180.0; //cos(180 + x) = -cos(x)
            else alpha = (T)360.0 - angle;

            if(alpha == (T)90.0) return (T)0.0;
        
            T beta = s.values[(size_t)(alpha / (T)90.0 * (T)N)];
            if(angle > (T)90.0 && angle < (T)270.0) return -beta;
            else return beta;
        }
    };


    template<size_t N, typename T = double> class TanTable {
        public:
        T values[N];
        constexpr TanTable() : values() {
            static_assert(false, "Not finished yet");
            static_assert(is_float<T>::value, "A tangent table requires a floating-point type");
            constexpr FactorialTable<maxN64BFact> f;

            size_t i;
            T alpha;
            for(i = 0, alpha = (T)0.0; i <= N; i++, alpha += (T)PI_L / ((T)2.0 * (T)N)) {
                T value = (T)0.0;
                size_t j, k;
                T gamma = alpha * alpha, beta = (T)1.0;
                for(j = 0, k = 0, beta = alpha; j <= maxN64BFact; j += 2, k++, beta *= gamma) {
                    if(k & 1) values[i] -= beta / (T)f.values[j];
                    else values[i] += beta / (T)f.values[j];
                }
            }
        }
    };

    //Not that accurate unfortunately
    /* template<double start, double end, double step, typename T = double> class ExpTable {
        public:
        T values[(size_t)((end - start) / step) + 1];
        constexpr ExpTable() : values() {
            static_assert(is_float<T>::value, "An e^x table requires a floating-point type");
            constexpr FactorialTable<maxN64BFact> f;

            int i;
            T x;
            for(i = 0, x = start; x <= end; x += step, i++) {
                values[i] = (T)1.0;
                size_t j;
                T t;
                for(j = 1, t = x; j <= maxN64BFact; j++, t *= x) {
                    values[i] += t / (T)f.values[j];
                }
            }
        }
    }; */
};