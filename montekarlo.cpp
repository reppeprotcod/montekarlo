#include<iostream>
#include<ctime>
using namespace std;

struct Point {
	double x, y;
};

double random(double min, double max)
{
	double x = (double)rand() / RAND_MAX;
	x *= max - min;
	x += min;
	return x;
}

bool is_in(double x, double y, double x1, double y1, double x2, double y2)
{
	double bx1 = x1, bx2 = x2;
	if (x2 < x1) {
		bx1 = x2;
		bx2 = x1;
	}

	double by1 = y1, by2 = y2;
	if (y2 < y1) {
		by1 = y2;
		by2 = y1;
	}

	if (x >= bx1 && x <= bx2 && y >= by1 && y <= by2) {
		return true;
	}
	return false;
}

bool intersect(Point p1, Point p2, Point p3, Point p4)
{
	/*double k1, k2, b1, b2;

	if (p1.x >= p2.x) {
		double x = p2.x;
		p2.x = p1.x;
		p1.x = x;
		double y = p2.y;
		p2.y = p1.y;
		p1.y = y;
	}

	if (p3.x >= p4.x) {
		double x = p3.x;
		p4.x = p3.x;
		p3.x = x;
		double y = p4.y;
		p4.y = p3.y;
		p3.y = y;
	}

	if (p1.x == p2.x && p3.y == p4.y) {
		if (p1.x >= p3.x && p1.x <= p4.x &&
			p3.y >= p2.y && p3.y <= p1.y) return true;
		else return false;
	}

	if (p3.x == p4.x && p1.y == p2.y) {
		if (p3.x >= p1.x && p3.x <= p2.x &&
			p1.y >= p3.y && p1.y <= p4.y) return true;
		else return false;
	}

	if (p1.y == p2.y) k1 = 0;
	else k1 = (p2.y - p1.y) / (p2.x - p1.x);

	if (p3.y == p4.y) k2 = 0;
	else k2 = (p4.y - p3.y) / (p4.x - p3.x);

	if (k1 == k2) return false;

	b1 = p1.y - k1 * p1.x;
	b2 = p3.y - k2 * p3.x;

	double x = (b2 - b1) / (k1 - k2);
	double y = k1 * x + b1;

	if (x >= p1.x && x <= p2.x && x >= p3.x && x <= p4.x)
		return true;
	else */return false;
}

// check if point inside polygon
bool is_in_poly(int n, Point *poly, Point p)
{
	bool c = false;
	for (int i = 0, j = n - 1; i < n; j = i++) {
		if ((((poly[i].y <= p.y) && (p.y < poly[j].y)) || ((poly[j].y <= p.y) && (p.y < poly[i].y))) &&
			(((poly[j].y - poly[i].y) != 0) && (p.x > ((poly[j].x - poly[i].x) * (p.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x))))
			c = !c;
	}
	return c;
}


bool is_in_circ(double x, double y, double cx, double cy, double r)
{
	if ((cx - x) * (cx - x) + (cy - y) * (cy - y) < r * r) {
		return true;
	}
	return false;
}

double plosh(int n, Point* poly)
{
	double bx1 = -100, by1 = -100, bx2 = 100, by2 = 100;
	double s = (bx2 - bx1) * (by2 - by1);

	int N = 1000000, k = 0;

	for (int i = 0; i < N; i++) {
		double x = random(bx1, bx2);
		double y = random(by1, by2);
		if (is_in_poly(n, poly, { x, y })) k++;
	}
	double s1 = (double)k / N * s;
	return s1;
}

double plosh_circ(double cx, double cy, double r)
{
	double bx1 = -100, by1 = -100, bx2 = 100, by2 = 100;
	double s = (bx2 - bx1) * (by2 - by1);
	
	int n = 1000000, k = 0;

	for (int i = 0; i < n; i++) {
		double x = random(bx1, bx2);
		double y = random(by1, by2);
		if (is_in_circ(x, y, cx, cy, r)) k++;
	}
	double s1 = (double)k / n * s;
	return s1;
}

int main()
{
	srand(time(0));

	int n = 0;
	printf("n:");
	scanf_s("%d", &n);
	Point* poly = new Point[n];

	for (int i = 0; i < n; i++) {
		printf("x:");
		scanf_s("%lf", &poly[i].x);
		printf("y:");
		scanf_s("%lf", &poly[i].y);
	}
	
	double s = plosh(n, poly);
	printf("%f\n", s);

	double cx = 0, cy = 0, r = 0;
	printf("cx:");
	scanf_s("%lf", &cx);
	printf("cy:");
	scanf_s("%lf", &cy);
	printf("r:");
	scanf_s("%lf", &r);

	s = plosh_circ(cx, cy, r);
	printf("%f\n", s);

	return 0;
}