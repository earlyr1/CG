make -f Makefile1 solve - обучает и тестирует на обучающей выборке.
Реализованы оба дополнительных задания. Весь написанный мною код находится в файте task2.cpp, в функциях:
	-float br(BMP* srcImage, int i, int j) - функция, возвращающая яркость пикселя;
	-void ExtractFeatures(const TDataSet& data_set, TFeatures* features) - функция, возвращающая признаки HOG, LBP и цветовые признаки единым вектором.
