#include "rotate.h"
#include <cmath>
#include <cstring>
#include <new> // for std::nothrow

/*
% INTERPOLATE ��ֵ����
% f ԭͼ szͼ���С
% m ԭͼy��������
% n ԭͼx��������
% ex x���������������
% ey x���������������
% way 1Ϊ����ڲ�ֵ��2Ϊ˫���Բ�ֵ����
*/
inline BYTE Interpolate(BYTE f[], int sz[], int m, int n, float ex, float ey, char way) {
	BYTE gray = 0;
	float fr1, fr2, fr3;

	// 1. ���ͳһ��0��1֮��
	if (ex < 0) {
		ex = 1 + ex;
		n--;
	}
	if (ey < 0) {
		ey = 1 + ey;
		m--;
	}
	if (m < 0 || n < 0 || m >= sz[0] - 1 || n >= sz[1] - 1) // �߽���
		return gray;

	// 2. ���ڽ���ֵ
	if (way == 1) {
		if (ex > 0.5)
			n++;
		if (ey > 0.5)
			m++;
		if (m >= sz[0] || n >= sz[1])
			return gray;
		gray = f[sz[1] * m + n];
		return gray;
	}

	// 3. ˫���Բ�ֵ
	if (way == 2) {
		if (m + 1 >= sz[0] || n + 1 >= sz[1]) // �߽���
			return gray;
		fr1 = (1 - ex) * float(f[sz[1] * m + n]) + ex * float(f[sz[1] * m + n + 1]);
		fr2 = (1 - ex) * float(f[sz[1] * (m + 1) + n]) + ex * float(f[sz[1] * (m + 1) + n + 1]);
		fr3 = (1 - ey) * fr1 + ey * fr2;
		gray = BYTE(fr3);
	}
	return gray;
}


BYTE* normalRoate(BYTE img[], int w, int h, double theta, int* neww, int* newh) {
	float fsin, fcos, c1, c2, fx, fy, ex, ey;
	int w1, h1, xx, yy;
	int sz[2] = { h,w };
	//1. �����������
	fsin = sin(theta);
	fcos = cos(theta);
	*newh = h1 = ceilf(abs(h * fcos) + abs(w * fsin));
	*neww = w1 = ceilf(abs(w * fcos) + abs(h * fsin));
	auto I1 = new(std::nothrow) BYTE[w1 * h1];
	if (!I1)
		return NULL;
	memset(I1, 0, w1 * h1);
	c1 = (w - w1 * fcos - h1 * fsin) / 2;
	c2 = (h + w1 * fsin - h1 * fcos) / 2;
	//2. ���㷴�����겢�����ֵ
	for (int y = 0; y < h1; y++) {
		for (int x = 0; x < w1; x++) {
			//�������ӳ���ľ�ȷλ�� ÿ���㶼ʹ��ԭʼ��ʽ����
			fx = x * fcos + y * fsin + c1; //�Ĵθ���˷����Ĵθ���ӷ�
			fy = y * fcos - x * fsin + c2;
			xx = roundf(fx);
			yy = roundf(fy);
			ex = fx - float(xx);
			ey = fy - float(yy);
			I1[w1 * y + x] = Interpolate(img, sz, yy, xx, ex, ey, 2);//˫���Բ�ֵ
		}
	}
	return I1;
}

BYTE* DDARoateFast(BYTE img[], int w, int h, double theta, int* neww, int* newh) {
	float fsin, fcos, c1, c2, fx, fy, ex, ey;
	int w1, h1, xx, yy;
	int sz[2] = { h, w };
	fsin = sin(theta);
	fcos = cos(theta);
	*newh = h1 = ceilf(abs(h * fcos) + abs(w * fsin));
	*neww = w1 = ceilf(abs(w * fcos) + abs(h * fsin));
	auto I1 = new(std::nothrow) BYTE[w1 * h1];
	if (!I1)
		return NULL;
	memset(I1, 0, w1 * h1);
	c1 = (w - w1 * fcos - h1 * fsin) / 2;
	c2 = (h + w1 * fsin - h1 * fcos) / 2;
	fx = c1 - fsin;
	fy = c2 - fcos;
	for (int y = 0; y < h1; y++) {
		fx += fsin;
		fy += fcos;
		xx = roundf(fx);
		yy = roundf(fy);
		ex = fx - float(xx);
		ey = fy - float(yy);
		for (int x = 0; x < w1; x++) {
			if (xx >= 0 && xx < sz[1] - 1 && yy >= 0 && yy < sz[0] - 1) {
				I1[w1 * y + x] = Interpolate(img, sz, yy, xx, ex, ey, 2);
			}
			ex += fcos;
			ey -= fsin;
			if (ex > 0.5) {
				xx++;
				ex -= 1;
			}
			if (ey < -0.5) {
				yy--;
				ey += 1;
			}
		}
	}
	return I1;
}

