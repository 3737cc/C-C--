#include "DspFilters/CoefficientRecovery.h"
#include <iostream>

template <int Order>
void Dsp::CoefficientRecovery::RecoveryCoefficient(Butterworth::Design::BandPass<Order>& bandPassFilter) {
    FilterCoefficients coeffs = GetFilterCoefficients(bandPassFilter);
    PrintFilterCoefficients(coeffs);
}

template <int Order>
Dsp::CoefficientRecovery::FilterCoefficients
Dsp::CoefficientRecovery::GetFilterCoefficients(Butterworth::Design::BandPass<Order>& bandPassFilter) {
    // 获取滤波器级数
    int numStages = bandPassFilter.getNumStages();

    // 初始化系数向量
    FilterCoefficients coeffs;
    coeffs.B_total.resize(2 * Order + 1, 0.0);
    coeffs.A_total.resize(2 * Order + 1, 0.0);

    // 遍历所有滤波器阶段并计算总系数
    for (int stage = 0; stage < numStages; ++stage) {
        // 创建临时系数向量来存储当前阶段的系数
        std::vector<double> B_stage(3, 0.0);
        std::vector<double> A_stage(3, 0.0);

        B_stage[0] = bandPassFilter[stage].getB0();
        B_stage[1] = bandPassFilter[stage].getB1();
        B_stage[2] = bandPassFilter[stage].getB2();
        A_stage[0] = bandPassFilter[stage].getA0();
        A_stage[1] = bandPassFilter[stage].getA1();
        A_stage[2] = bandPassFilter[stage].getA2();

        // 如果是第一个阶段，直接复制
        if (stage == 0) {
            coeffs.B_total = B_stage;
            coeffs.A_total = A_stage;
        }
        else {
            // 对于后续阶段，进行卷积运算
            std::vector<double> B_temp(coeffs.B_total.size() + B_stage.size() - 1, 0.0);
            std::vector<double> A_temp(coeffs.A_total.size() + A_stage.size() - 1, 0.0);

            // B系数卷积
            for (size_t i = 0; i < coeffs.B_total.size(); ++i) {
                for (size_t j = 0; j < B_stage.size(); ++j) {
                    B_temp[i + j] += coeffs.B_total[i] * B_stage[j];
                }
            }

            // A系数卷积
            for (size_t i = 0; i < coeffs.A_total.size(); ++i) {
                for (size_t j = 0; j < A_stage.size(); ++j) {
                    A_temp[i + j] += coeffs.A_total[i] * A_stage[j];
                }
            }

            coeffs.B_total = std::move(B_temp);
            coeffs.A_total = std::move(A_temp);
        }
    }

    return coeffs;
}

void Dsp::CoefficientRecovery::PrintFilterCoefficients(const FilterCoefficients& coeffs) {
    // 输出结果
    std::cout << "B_total: ";
    for (double b : coeffs.B_total) {
        std::cout << b << " ";
    }
    std::cout << "\n";
    std::cout << "A_total: ";
    for (double a : coeffs.A_total) {
        std::cout << a << " ";
    }
    std::cout << "\n";
}

// RecoveryCoefficient 实例化
template void Dsp::CoefficientRecovery::RecoveryCoefficient<1>(Butterworth::Design::BandPass<1>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<2>(Butterworth::Design::BandPass<2>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<3>(Butterworth::Design::BandPass<3>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<4>(Butterworth::Design::BandPass<4>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<5>(Butterworth::Design::BandPass<5>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<6>(Butterworth::Design::BandPass<6>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<7>(Butterworth::Design::BandPass<7>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<8>(Butterworth::Design::BandPass<8>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<9>(Butterworth::Design::BandPass<9>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<10>(Butterworth::Design::BandPass<10>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<11>(Butterworth::Design::BandPass<11>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<12>(Butterworth::Design::BandPass<12>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<13>(Butterworth::Design::BandPass<13>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<14>(Butterworth::Design::BandPass<14>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<15>(Butterworth::Design::BandPass<15>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<16>(Butterworth::Design::BandPass<16>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<17>(Butterworth::Design::BandPass<17>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<18>(Butterworth::Design::BandPass<18>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<19>(Butterworth::Design::BandPass<19>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<20>(Butterworth::Design::BandPass<20>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<21>(Butterworth::Design::BandPass<21>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<22>(Butterworth::Design::BandPass<22>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<23>(Butterworth::Design::BandPass<23>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<24>(Butterworth::Design::BandPass<24>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<25>(Butterworth::Design::BandPass<25>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<26>(Butterworth::Design::BandPass<26>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<27>(Butterworth::Design::BandPass<27>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<28>(Butterworth::Design::BandPass<28>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<29>(Butterworth::Design::BandPass<29>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<30>(Butterworth::Design::BandPass<30>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<31>(Butterworth::Design::BandPass<31>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<32>(Butterworth::Design::BandPass<32>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<33>(Butterworth::Design::BandPass<33>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<34>(Butterworth::Design::BandPass<34>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<35>(Butterworth::Design::BandPass<35>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<36>(Butterworth::Design::BandPass<36>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<37>(Butterworth::Design::BandPass<37>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<38>(Butterworth::Design::BandPass<38>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<39>(Butterworth::Design::BandPass<39>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<40>(Butterworth::Design::BandPass<40>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<41>(Butterworth::Design::BandPass<41>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<42>(Butterworth::Design::BandPass<42>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<43>(Butterworth::Design::BandPass<43>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<44>(Butterworth::Design::BandPass<44>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<45>(Butterworth::Design::BandPass<45>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<46>(Butterworth::Design::BandPass<46>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<47>(Butterworth::Design::BandPass<47>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<48>(Butterworth::Design::BandPass<48>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<49>(Butterworth::Design::BandPass<49>& bandPassFilter);
template void Dsp::CoefficientRecovery::RecoveryCoefficient<50>(Butterworth::Design::BandPass<50>& bandPassFilter);

// GetFilterCoefficients 实例化
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<1>(Butterworth::Design::BandPass<1>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<2>(Butterworth::Design::BandPass<2>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<3>(Butterworth::Design::BandPass<3>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<4>(Butterworth::Design::BandPass<4>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<5>(Butterworth::Design::BandPass<5>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<6>(Butterworth::Design::BandPass<6>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<7>(Butterworth::Design::BandPass<7>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<8>(Butterworth::Design::BandPass<8>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<9>(Butterworth::Design::BandPass<9>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<10>(Butterworth::Design::BandPass<10>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<11>(Butterworth::Design::BandPass<11>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<12>(Butterworth::Design::BandPass<12>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<13>(Butterworth::Design::BandPass<13>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<14>(Butterworth::Design::BandPass<14>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<15>(Butterworth::Design::BandPass<15>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<16>(Butterworth::Design::BandPass<16>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<17>(Butterworth::Design::BandPass<17>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<18>(Butterworth::Design::BandPass<18>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<19>(Butterworth::Design::BandPass<19>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<20>(Butterworth::Design::BandPass<20>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<21>(Butterworth::Design::BandPass<21>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<22>(Butterworth::Design::BandPass<22>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<23>(Butterworth::Design::BandPass<23>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<24>(Butterworth::Design::BandPass<24>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<25>(Butterworth::Design::BandPass<25>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<26>(Butterworth::Design::BandPass<26>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<27>(Butterworth::Design::BandPass<27>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<28>(Butterworth::Design::BandPass<28>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<29>(Butterworth::Design::BandPass<29>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<30>(Butterworth::Design::BandPass<30>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<31>(Butterworth::Design::BandPass<31>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<32>(Butterworth::Design::BandPass<32>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<33>(Butterworth::Design::BandPass<33>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<34>(Butterworth::Design::BandPass<34>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<35>(Butterworth::Design::BandPass<35>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<36>(Butterworth::Design::BandPass<36>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<37>(Butterworth::Design::BandPass<37>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<38>(Butterworth::Design::BandPass<38>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<39>(Butterworth::Design::BandPass<39>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<40>(Butterworth::Design::BandPass<40>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<41>(Butterworth::Design::BandPass<41>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<42>(Butterworth::Design::BandPass<42>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<43>(Butterworth::Design::BandPass<43>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<44>(Butterworth::Design::BandPass<44>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<45>(Butterworth::Design::BandPass<45>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<46>(Butterworth::Design::BandPass<46>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<47>(Butterworth::Design::BandPass<47>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<48>(Butterworth::Design::BandPass<48>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<49>(Butterworth::Design::BandPass<49>& bandPassFilter);
template Dsp::CoefficientRecovery::FilterCoefficients Dsp::CoefficientRecovery::GetFilterCoefficients<50>(Butterworth::Design::BandPass<50>& bandPassFilter);
