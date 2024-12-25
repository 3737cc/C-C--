#ifndef COEFFICIENT_RECOVERY_H
#define COEFFICIENT_RECOVERY_H

#include <vector>
#include "DspFilters/Dsp.h"

namespace Dsp {
    namespace CoefficientRecovery {
        // 定义结构体存储系数
        struct FilterCoefficients {
            std::vector<double> B_total;
            std::vector<double> A_total;
        };

        // 原有的打印系数函数
        template <int Order>
        void RecoveryCoefficient(Butterworth::Design::BandPass<Order>& bandPassFilter);

        // 新增函数：获取系数并返回
        template <int Order>
        FilterCoefficients GetFilterCoefficients(Butterworth::Design::BandPass<Order>& bandPassFilter);

        // 新增函数：打印系数
        void PrintFilterCoefficients(const FilterCoefficients& coeffs);
    }
}

#endif // COEFFICIENT_RECOVERY_H