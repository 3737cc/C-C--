
//// 更改阶数
//constexpr int FilterOrder = 10;

//// 带通滤波器参数设置
//constexpr double i_fs = 1500.0;     // 采样率HZ
//constexpr double i_f1 = 500.0;      // 截至下限频率HZ
//constexpr double i_f2 = 560.0;      // 截至上限频率HZ

//Butterworth::Design::BandPass<FilterOrder> bandPassFilter;

//// 计算中心频率和带宽
//double i_dCenterFrequency = (i_f1 + i_f2) / 2;    // 中心频率
//double i_dBandwidth = i_f2 - i_f1;                // 带宽

//// 设置滤波器参数
//bandPassFilter.setup(FilterOrder, i_fs, i_dCenterFrequency, i_dBandwidth);

//// 直接打印系数
//CoefficientRecovery::RecoveryCoefficient(bandPassFilter);
//// 获取系数并保存在coeffs中
//auto coeffs = CoefficientRecovery::GetFilterCoefficients(bandPassFilter);
//std::cout << "A系数：\n";
//for (size_t i = 0; i < coeffs.A_total.size(); ++i) {
//    std::cout << "A[" << i << "]: " << coeffs.A_total[i] << "\n";
//}
//std::cout << "B系数：\n";
//for (size_t i = 0; i < coeffs.B_total.size(); ++i) {
//    std::cout << "B[" << i << "]: " << coeffs.B_total[i] << "\n";
//}

//// 使用声明循环卷积函数
//// 加载图像，读取为灰度图像
//cv::Mat pImage = cv::imread("0001.tif", cv::IMREAD_GRAYSCALE);
//if (pImage.empty()) {
//    std::cerr << "Error: Unable to load image!" << std::endl;
//    return -1;
//}

//int type= pImage.type();
//std::string typeName = getMatTypeName(type);
//uint8_t i = 1;
//pImage.forEach<uint8_t>([](uint8_t& pixel, const int* position) -> void {
//    if (pixel > 1) pixel = 1;
//    else if (pixel < 0) pixel = 0;
//    });
////图像归一化
//cv::Mat pImageFloat ;
//pImage.convertTo(pImageFloat, CV_32F, 1.0 / 255.0);

//// 自定义卷积核 (5x5)
//cv::Mat pCustomKernel = (cv::Mat_<double>(5, 5) <<
//    0.0317564010287247, 0.0375157550304212, 0.0396589455082867, 0.0375157550304212, 0.0317564010287247,
//    0.0375157550304212, 0.0443196278517048, 0.0468515082394857, 0.0443196278517048, 0.0375157550304212,
//    0.0396589455082867, 0.0468515082394857, 0.0495280292438231, 0.0468515082394857, 0.0396589455082867,
//    0.0375157550304212, 0.0443196278517048, 0.0468515082394857, 0.0443196278517048, 0.0375157550304212,
//    0.0317564010287247, 0.0375157550304212, 0.0396589455082867, 0.0375157550304212, 0.0317564010287247
//    );

//// 设置模式
//const std::string pMode = "circular";

//cv::Mat pA = (cv::Mat_<float>(2, 2) <<
//    1.0f, 2.0f,
//    2.0f, 1.0f
//    );

//cv::Mat pB = (cv::Mat_<float>(4, 4) <<
//    1.0f, 2.0f, 1.0f, 2.0f,
//    2.0f, 1.0f, 2.0f, 1.0f,
//    1.0f, 2.0f, 1.0f, 4.0f,
//    2.0f, 1.0f, 2.0f, 3.0f
//    );
//cv::Mat C = (cv::Mat_<float>(2, 16) << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
//1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
//cv::Mat A = (cv::Mat_<float>(2, 2) <<
//0.2f, 0.2f,
//0.3f, 0.1f
//);
// 执行自定义卷积函数
//cv::Mat pResponse = customConvolution(pImageFloat, pCustomKernel, pMode);
//
//// 执行 valid 卷积
//cv::Mat pResult = conv2_valid(C, A);

//std::cout << pResult << std::endl;

// 保存结果为 TXT 文件，使用高精度输出
//std::ofstream file("pResult_image.txt");
//file << std::fixed << std::setprecision(9);  // 设置高精度输出
//for (int i = 0; i < pResult.rows; ++i) {
//    for (int j = 0; j < pResult.cols; ++j) {
//        file << pResult.at<float>(i, j) << "\t";
//    }
//    file << "\n";
//}
//file.close();

//cv::Mat pSameImageDouble;
//pImage.convertTo(pSameImageDouble, CV_64F);

//// 执行 Same卷积操作
//cv::Mat pSame;
//cv::filter2D(pSameImageDouble, pSame, -1, pCustomKernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);

//// 保存结果为 TXT 文件，使用高精度输出
//std::ofstream pSameFile("pSame_image.txt");
//pSameFile << std::fixed << std::setprecision(9);  // 设置高精度输出
//for (int i = 0; i < pSame.rows; ++i) {
//    for (int j = 0; j < pSame.cols; ++j) {
//        pSameFile << pSame.at<double>(i, j) << "\t";
//    }
//    pSameFile << "\n";
//}
//pSameFile.close();

//// 输入矩阵
//cv::Mat mat = (cv::Mat_<float>(2, 2) << 0.5, 0.3, -0.7, 1.0);

//// 确保数据在反余弦的合法输入范围 [-1, 1] 之间
//cv::Mat matNormalized;
//cv::normalize(mat, matNormalized, -1, 1, cv::NORM_MINMAX);  // 归一化到 [-1, 1] 范围

//// 创建一个结果矩阵
//cv::Mat result = matNormalized.clone();

//// 对每个元素应用 std::acos
//result.forEach<float>([](float& pixel, const int* position) -> void {
//    pixel = std::acos(pixel);
//    });

//cv::Mat image = (cv::Mat_<float>(2, 4) << 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f);
//cv::Mat kernel = (cv::Mat_<float>(2, 2) << 0.2f, 0.2f, 0.3f, 0.1f);
//cv::Mat C = (cv::Mat_<float>(2, 16) << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
//    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
//cv::Mat D = (cv::Mat_<float>(2, 2) << 0.2f, 0.2f, 0.3f, 0.1f);
////cv::Mat pB = (cv::Mat_<float>(4, 4) <<
////    1.0f, 2.0f, 1.0f, 2.0f,
////    2.0f, 1.0f, 2.0f, 1.0f,
////    1.0f, 2.0f, 1.0f, 4.0f,
////    2.0f, 1.0f, 2.0f, 3.0f
////    );
////cv::Mat pA = (cv::Mat_<float>(2, 2) <<
////    1.0f, 2.0f,
////    2.0f, 1.0f
////    );

//cv::Mat result;
//////执行二维卷积
////cv::filter2D(image, result, -1, kernel);
//// 执行二维卷积，确保边界不进行填充
//cv::filter2D(C, result, -1, D, cv::Point(-1, -1), 0, cv::BORDER_REFLECT);
////std::cout << result << std::endl;
//// 计算 valid 区域的大小
//int borderX = D.cols / 2;
//int borderY = D.rows / 2;

//// 裁剪 valid 区域，去除边界部分
//result = result(cv::Rect(borderX, borderY, C.cols - D.cols + 1, image.rows - D.rows + 1));

//std::cout << result << std::endl;
// // 执行 valid 卷积
//cv::Mat result = conv2_valid(image, kernel);
// 保存结果为 TXT 文件，使用高精度输出
//std::ofstream ifile("result_image.txt");
//ifile << std::fixed << std::setprecision(9);  // 设置高精度输出
//for (int i = 0; i < result.rows; ++i) {
//    for (int j = 0; j < result.cols; ++j) {
//        ifile << result.at<float>(i, j) << "\t";
//    }
//    ifile << "\n";
//}
//ifile.close();
