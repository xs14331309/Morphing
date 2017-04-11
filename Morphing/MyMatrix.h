#include <iostream>
#include <stdlib.h>

#define MYDIM 3    //测试矩阵维数定义

extern double** getAMatrix(int row, int col) {
	double **matrix = new double*[col];
	for (int i = 0; i < row; i++) {
		matrix[i] = new double[3];
		memset(matrix[i], 0, 3 * sizeof(double));
	}
	return matrix;
}

extern double** makeAMatrix(Point &p0, Point &p1, Point &p2) {
	double** m = getAMatrix(3, 3);
	m[0][0] = p0.x;
	m[0][1] = p1.x;
	m[0][2] = p2.x;
	m[1][0] = p0.y;
	m[1][1] = p1.y;
	m[1][2] = p2.y;
	m[2][0] = 1;
	m[2][1] = 1;
	m[2][2] = 1;
	return m;
}

extern void deleteAMatrix(double **matrix) {
	for (int i = 0; i < 3; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

extern double getA(double** arcs, int n) {
	if (n == 1) {
		return arcs[0][0];
	}
	double ans = 0;
	double** temp = getAMatrix(MYDIM, MYDIM);
	int i, j, k;
	for (i = 0; i<n; i++) {
		for (j = 0; j<n - 1; j++) {
			for (k = 0; k<n - 1; k++) {
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

			}
		}
		double t = getA(temp, n - 1);
		if (i % 2 == 0) {
			ans += arcs[0][i] * t;
		}
		else {
			ans -= arcs[0][i] * t;
		}
	}
	deleteAMatrix(temp);
	return ans;
}

//计算每一行每一列的每个元素所对应的余子式，组成A*
extern void getAStart(double** arcs, int n, double** ans) {
	if (n == 1) {
		ans[0][0] = 1;
		return;
	}
	int i, j, k, t;
	double** temp = getAMatrix(MYDIM, MYDIM);
	for (i = 0; i<n; i++) {
		for (j = 0; j<n; j++) {
			for (k = 0; k<n - 1; k++) {
				for (t = 0; t<n - 1; t++) {
					temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}


			ans[j][i] = getA(temp, n - 1);  //此处顺便进行了转置
			if ((i + j) % 2 == 1) {
				ans[j][i] = -ans[j][i];
			}
		}
	}
	deleteAMatrix(temp);
}

//得到给定矩阵src的逆矩阵保存到des中。
extern bool GetMatrixInverse(double** src, int n, double** des) {
	double flag = getA(src, n);
	double** t = getAMatrix(MYDIM, MYDIM);
	if (0 == flag) {
		std::cout << "原矩阵行列式为0，无法求逆。请重新运行" << std::endl;
		return false;//如果算出矩阵的行列式为0，则不往下进行
	}
	else {
		getAStart(src, n, t);
		for (int i = 0; i<n; i++) {
			for (int j = 0; j<n; j++) {
				des[i][j] = t[i][j] / flag;
			}
		}
	}
	deleteAMatrix(t);
	return true;
}



extern double** getInverse(double** matrix_before) {
	bool flag;//标志位，如果行列式为0，则结束程序

	double **matrix_after = getAMatrix(3, 3);
	flag = GetMatrixInverse(matrix_before, MYDIM, matrix_after);
	if (false == flag)
		return NULL;

	return matrix_after;
}

extern double** getMatrixProduct(double** m1, double** m2) {
	double** ans = getAMatrix(MYDIM, MYDIM);
	for (int i = 0; i<MYDIM; i++) {
		for (int j = 0; j<MYDIM; j++) {
			for (int k = 0; k<MYDIM; k++) {
				ans[i][j] += m1[i][k] * m2[k][j];
			}
			if (abs(ans[i][j]) < pow(0.1, 5))
				ans[i][j] = 0;
		}
	}
	return ans;
}
