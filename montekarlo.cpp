#define _USE_MATH_DEFINES

#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <ctime>

using namespace std;

struct vertex {
	double x;
	double y;
};

struct point {
	int x;
	int y;
};

typedef vector<vertex> polygon;

#define VB_SIZE 40
#define VB_EDGE '*'
#define VB_FILL '@'
#define VB_VOID ' '

typedef char pixel;
typedef pixel** vbuffer;

enum class rasterState {
	SEARCH,
	FILL,
	CLEAN,
	END
};

double random(double min, double max)
{
	double x = (double)rand() / RAND_MAX;
	return x * (max - min) + min;
}

pixel getPixel(vbuffer& buffer, int x, int y)
{
	if (x >= 0 && x < VB_SIZE && y >= 0 && y < VB_SIZE)
		return buffer[y][x];
	else return 0;
}

void setPixel(vbuffer& buffer, int x, int y, pixel value)
{
	if (x >= 0 && x < VB_SIZE && y >= 0 && y < VB_SIZE)
		buffer[y][x] = value;
}

void initVideoBuffer(vbuffer& buffer)
{
	buffer = new pixel * [VB_SIZE];
	for (int i = 0; i < VB_SIZE; i++) {
		buffer[i] = new pixel[VB_SIZE];
		pixel value = (i == VB_SIZE - 1) ? '-' : VB_VOID;// выбор символа для заполнения строки
		memset(buffer[i], value, VB_SIZE * sizeof(pixel));//заполнения строки
		setPixel(buffer, 0, i, '|');
	}
	setPixel(buffer, 0, 0, '^');
	setPixel(buffer, 0, VB_SIZE - 1, '+');
	setPixel(buffer, VB_SIZE - 1, VB_SIZE - 1, '>');
}

void displayVideoBuffer(const vbuffer& buffer)
{
	for (int y = 0; y < VB_SIZE; y++) {
		for (int x = 0; x < VB_SIZE; x++) {
			putchar(buffer[y][x]);
			if (x > 0 && x < VB_SIZE - 1)//дублирвоание символа для изображения в консоли,
				putchar(buffer[y][x]);  // кроме первого и последнего столбца 
		}
		if (y != VB_SIZE - 1) putchar('\n');
	}
}

//рисование линии по алгоритму Брезенхема
void drawLine(vbuffer& buffer, point a, point b, pixel value) {
	int dx = abs(b.x - a.x);
	int sx = a.x < b.x ? 1 : -1;
	int dy = abs(b.y - a.y);
	int sy = a.y < b.y ? 1 : -1;

	int err = (dx > dy ? dx : -dy) / 2;
	int e2 = 0;

	for (;;) {
		setPixel(buffer, a.x, a.y, value);
		if (a.x == b.x && a.y == b.y) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; a.x += sx; }
		if (e2 < dy) { err += dx; a.y += sy; }
	}
}

//заполнение фигуры
void rasterFill(vbuffer& buffer)
{
	rasterState state = rasterState::SEARCH;
	for (int y = 0; y < VB_SIZE - 1; y++) {
		for (int x = 0;;) {//заполнение производится построчно
			if (state == rasterState::SEARCH) {//стадия поиска левой границы
				if (getPixel(buffer, x, y) == VB_EDGE &&
					getPixel(buffer, x + 1, y) != VB_EDGE)
					state = rasterState::FILL;//переход на стадию заполнения
				if (++x == VB_SIZE - 1) break;
			}
			else if (state == rasterState::FILL) {//стадия заполнения
				if (getPixel(buffer, x, y) == VB_EDGE)
					state = rasterState::END;//завершение заполнения
				else setPixel(buffer, x, y, VB_FILL);//заполнение
				if (++x == VB_SIZE) state = rasterState::CLEAN;//переход на стадию очистки
			}
			else if (state == rasterState::CLEAN) {
				if (getPixel(buffer, x, y) == VB_EDGE) break;
				setPixel(buffer, x, y, VB_VOID);//очистка
				x--;
			}
			else if (state == rasterState::END) break;
		}
		state = rasterState::SEARCH;
	}
}

void rasterizePolygon(vbuffer& buffer, const polygon& poly)
{
	//отрисовка границ многоугольника
	for (int i = 0, j = poly.size() - 1; i < poly.size(); j = i++) {
		point a = { floor(poly[j].x), floor(VB_SIZE - poly[j].y) };
		point b = { floor(poly[i].x), floor(VB_SIZE - poly[i].y) };
		drawLine(buffer, a, b, VB_EDGE);
	}
	rasterFill(buffer);
}

void scalePolygon(polygon& poly, double factor)
{
	for_each(poly.begin(), poly.end(),
		[factor](vertex& v) {
			v.x *= factor;
			v.y *= factor;
		});
}

polygon randomPolygon(double maxCoord, int vertexNum)
{
	polygon poly;

	//параметры окружности
	double x0 = maxCoord / 2;
	double y0 = maxCoord / 2;
	double radius = x0;

	double a0 = 0.75 * (2 * M_PI / vertexNum);//минимальное отклонение угла
	double a1 = 1.15 * (2 * M_PI / vertexNum);//максимальное отклонение угла
	double startAngle = random(-M_PI / 2, M_PI / 2);//начальный угол

	int N = 0;//количесвто построенных вершин
	double angle = startAngle;//текущий угол
	while (angle < M_PI * 2 + startAngle) {
		poly.push_back({//вычисление координат вершины
			x0 + radius * cos(angle),
			y0 + radius * sin(angle)
			});
		angle += random(a0, a1);
		if (++N == vertexNum) break;
	}

	return poly;
}

//быстрый алгоритм проверки принадлежности вершины многоугольнику
bool insidePolygon(const polygon& p, const vertex& v)
{
	bool c = false;
	for (int i = 0, j = p.size() - 1; i < p.size(); j = i++) {
		if ((((p[i].y <= v.y) && (v.y < p[j].y)) || ((p[j].y <= v.y) && (v.y < p[i].y))) &&
			(((p[j].y - p[i].y) != 0) && (v.x > ((p[j].x - p[i].x) * (v.y - p[i].y) / (p[j].y - p[i].y) + p[i].x))))
			c = !c;
	}
	return c;
}

double polygonArea(const polygon& poly, double maxCoord, int samplesNum)
{
	double totalArea = pow(maxCoord, 2);
	int k = 0;//количесвто попаданий внутрь многоугольника

	for (int i = 0; i < samplesNum; i++) {
		vertex sample = { random(0, maxCoord), random(0, maxCoord) };//случайная точка
		if (insidePolygon(poly, sample)) k++;
	}

	return (double)k / samplesNum * totalArea;
}

void rasterizeCircle(vbuffer& buffer, double radius)
{
	double center = VB_SIZE - radius;
	//начальные точки линий границ окружности
	point a1 = { 0, center };
	point a2 = { 0, center };
	for (int i = 1; i <= radius * 2; i++) {
		double offset = sqrt(pow(radius, 2) - pow(i - radius, 2));
		//конечные точки линий границ окружности
		point b1 = { i, center + offset };
		point b2 = { i, center - offset };
		drawLine(buffer, a1, b1, VB_EDGE);
		drawLine(buffer, a2, b2, VB_EDGE);
		a1 = b1;
		a2 = b2;
	}
	rasterFill(buffer);
}

bool insideCircle(double radius, const vertex& v)
{
	return pow(v.x - radius, 2) + pow(v.y - radius, 2) < pow(radius, 2);
}

double circleArea(double radius, int samplesNum)
{
	double totalArea = pow(radius * 2, 2);
	int k = 0;

	for (int i = 0; i < samplesNum; i++) {
		vertex sample = { random(0, radius * 2), random(0, radius * 2) };
		if (insideCircle(radius, sample)) k++;
	}

	return (double)k / samplesNum * totalArea;
}

void rasterizeFunction(vbuffer& buffer, double a, double b, double maxCoord)
{
	double scaleFactor = VB_SIZE / maxCoord;//масштаб
	for (int i = 1; i < VB_SIZE; i++) {
		double coord = VB_SIZE - (a + b * sin(i / scaleFactor)) * scaleFactor;//значение функции по заданному х
		point pa = { i, VB_SIZE - 2 };
		point pb = { i, coord };
		drawLine(buffer, pa, pb, VB_FILL);
		setPixel(buffer, i, coord, VB_EDGE);
	}
}

bool insideFunction(double a, double b, const vertex& v)
{
	return v.y < (a + b * sin(v.x));
}

double functionArea(double a, double b, double maxCoord, int samplesNum)
{
	double totalArea = pow(maxCoord, 2);
	int k = 0;

	for (int i = 0; i < samplesNum; i++) {
		vertex sample = { random(0, maxCoord), random(0, maxCoord) };
		if (insideFunction(a, b, sample)) k++;
	}

	return (double)k / samplesNum * totalArea;
}

int main()
{
	srand(time(0));

	double max = 0;
	int samples = 0;
	int type = 0;

	printf("Enter shape (0 - polygon, 1 - circle, 2 - a+b*sin(x)): ");
	cin >> type;
	while (type < 0 || type > 2) {
		printf("Enter valid number: ");
		cin >> type;
	}

	printf("Enter maximum coordinate (>=1): ");
	cin >> max;
	while (max < 1) {
		printf("Enter valid number: ");
		cin >> max;
	}

	printf("Enter number of samples (>=1000): ");
	cin >> samples;
	while (samples < 1000) {
		printf("Enter valid number: ");
		cin >> samples;
	}

	vbuffer buffer;
	initVideoBuffer(buffer);

	if (type == 0) {
		int vertices = 0;
		printf("Enter number of vertices (>=3): ");
		cin >> vertices;
		while (vertices < 3) {
			printf("Enter valid number: ");
			cin >> vertices;
		}

		polygon poly = randomPolygon(max, vertices);
		double area = polygonArea(poly, max, samples);
		printf("\nTotal area = %.3f\nPolygon area = %.3f\n\n", pow(max, 2), area);

		double scaleFactor = (double)VB_SIZE / max;
		scalePolygon(poly, scaleFactor);
		rasterizePolygon(buffer, poly);
	}
	else if (type == 1) {
		double area = circleArea(max / 2, samples);
		printf("\nTotal area = %.3f\nCircle area = %.3f\n\n", pow(max, 2), area);
		rasterizeCircle(buffer, (double)VB_SIZE / 2 - 1);
	}
	else if (type == 2) {
		double a = 0;
		double b = 0;

		printf("Enter a: ");
		cin >> a;
		printf("Enter b: ");
		cin >> b;

		double area = functionArea(a, b, max, samples);
		printf("\nTotal area = %.3f\nFunction area = %.3f\n\n", pow(max, 2), area);
		rasterizeFunction(buffer, a, b, max);
	}

	printf("%.1f\n", max);//подпись оси ординат
	displayVideoBuffer(buffer);
	printf(" %.1f\n", max);//подпись оси абсцисс

	system("pause");
}
