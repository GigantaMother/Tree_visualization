#include <iostream>
#include <map>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>

#define clear() printf("\033[H\033[J")

using namespace std;

// Структура для дерева
struct node
{
	int		info;	// ключ
	unsigned char	height;	// высота(глубина) дерева
	int		layer;	// слой, на котором находится элемент
	node		*l, *r;	// левая, правая ветка
};

//=============================================================================================================================================================
//==================================================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ДЕРЕВА =====================================================================
//=============================================================================================================================================================

// Возвращает высоту дерева, даже если узел NULL
unsigned char height(node* p)
{
	return (p ? p->height : 0);
}

// Востанавливает корректность высоты, при условии, что у веток верная высота
unsigned char fixheight(node* p)
{
	unsigned char hl = height(p->l);
	unsigned char hr = height(p->r);

	return ((hl > hr ? hl : hr) + 1);
}

// Считает кол-во элементов в дереве
int size(node *tree)
{
	int s = 0;

	if (tree == NULL)
		return (0);
	s++;

	s += size(tree->l);
	s += size(tree->r);

	return (s);
}

// Считает глубину дерева
int depth(node *tree)
{
	if (tree == NULL)
		return (0);

	int leftDepth = depth(tree->l);
	int rightDepth = depth(tree->r);

	return (leftDepth > rightDepth ? leftDepth + 1 : rightDepth + 1);
}

// Переводит число в строку (число, символ заполнения если длина числа меньше)
std::string print_num(int num, char ch)
{
	std::string temp = "";

	if (num < 0 || num > 999)
		temp += "???";
	else
	{
		if (num < 10)
			temp = temp + ch + ch;
		else if (num < 100)
			temp += ch;
		temp += to_string(num);
	}
	return (temp);
}

// Добавить элемент в дерево, пересчитать все пераметры узла
void push(int a, node **t)
{
	if ((*t) == NULL)					// Деоева нет
	{
		(*t) = new node;				// Выделяем память
		(*t)->info = a;					// Кладем в выделенное место аргумент a
		(*t)->l = (*t)->r = NULL;		// Очищаем память для следующего роста
		(*t)->height = 1;
		(*t)->layer = 0;

		return;
	}
	if (a > (*t)->info)					// Дерево есть
	{
		push(a, &(*t)->r);
		(*t)->r->layer = ((*t)->layer) + 1;
	}
	else
	{
		push(a, &(*t)->l);
		(*t)->l->layer = ((*t)->layer) + 1;
	}
	(*t)->height = fixheight(*t);		// Корректируем глубину
}

//=============================================================================================================================================================
//================ РЕЖИМ ПЕЧАТИ ДЕРЕВА С БУФЕРОМ (ДЕРЕВО НЕМНОГО МЕНЬШЕ ПО ШИРИНЕ, НО ПЕЧАТАЕТСЯ ДОЛГО, ЕСТЬ РЕЖИМЫ ПЕЧАТИ ДОП ДАННЫХ)=========================
//=============================================================================================================================================================

// Правильно добавляет данные для печати (tree - дерево, lev_tree массив строк для записи элементов дерева,
// lev_tree_height - массив строк для записи доп. данных(глубина, слой), lev - слой дерева, mod - нужный отступ,
// mod_h - режим доп. данных(1 - добовлять глубину, 2 - добавлять слой, по умолчанию ничего не добавляется))
void fill_depth(node *tree, std::string *lev_tree, std::string *lev_tree_height, int lev, int mod, int mod_h = 0)
{
	if (tree == NULL)
		return ;

	int	d = depth(tree), skip_pred = pow(2, d - 1) - 2, arm = pow(2, d - 1);
	string	num_str = print_num(tree->info, '0'), height_str;

	if (mod == -1)
		mod = pow(2, d) - 2;

	if (mod_h == 1)
		height_str = print_num(tree->height, '.');
	else if (mod_h == 2)
		height_str = print_num(tree->layer, '.');
	
	lev_tree[lev].replace(mod, 3, num_str);
	if (mod_h)
		lev_tree_height[lev].replace(mod, 3, height_str);

	lev++;
	if (tree->l != NULL)
	{
		if (mod_h)
			lev_tree_height[lev - 1].replace(mod - arm, arm, arm, '_');
		else
			lev_tree[lev - 1].replace(mod - arm, arm, arm, '_');
		fill_depth(tree->l, lev_tree, lev_tree_height, lev, mod - skip_pred - 2, mod_h);
	}
	if (tree->r != NULL)
	{
		if (mod_h)
			lev_tree_height[lev - 1].replace(mod + 3, arm, arm, '_');
		else
			lev_tree[lev - 1].replace(mod + 3, arm, arm, '_');
		fill_depth(tree->r, lev_tree, lev_tree_height, lev, mod + skip_pred + 2, mod_h);
	}
}

// Удаляет пустые столбцы из массива строк
void delete_columns(string	*lev_tree, string *lev_tree_height, int skip, int d, int mod_h = 0)
{
	int		start = -1, end = -1, size_del = -1;
	bool	flag = false;

	for (int i = skip - 1; i >= 0; i--)
	{
		flag = true;

		for (size_t j = 0; j < d; j++)
		{
			if (lev_tree[j][i] != ' ' && lev_tree[j][i] != '_')
			{
				flag = false;
				break ;
			}
		}
		if (flag == true && start == -1)
			start = i;
		if (flag == false && start != -1)
		{
			end = i;
			size_del = start - i;
			for (size_t j = 0; j < d; j++)
			{
				lev_tree[j].erase(end + 1, size_del);
				if (mod_h == 1)
					lev_tree_height[j].erase(end + 1, size_del);
			}
			start = end = -1;
		}
	}
	if (start != - 1)
	{
		size_del = start - end;
		for (size_t j = 0; j < d; j++)
		{
			lev_tree[j].erase(0, size_del);
			if (mod_h == 1)
				lev_tree_height[j].erase(0, size_del);
		}
	}
}

// Печать дерева МЕТОД С БУФЕРОМ (tree - дерево, mod_h - режим доп. данных(1 - добовлять глубину, 2 - добавлять слой, по умолчанию ничего не добавляется))
void print_tree(node *tree, int mod_h = 0)
{
	int		time_tree = clock(), d = depth(tree), skip = (pow(2, d) - 2) * 2 + 3;
	string	lev_tree[d], lev_tree_height[d];

	cout << "Tree buf\n" << "d= " << d << endl << "s= " << size(tree) << endl;
	// cout << "skip(max)= " << skip << endl;

	// Заполняем строки пробелами
	if (d > 0)
	{
		lev_tree[0].insert(0, skip, ' ');
		if (mod_h == 1)
			lev_tree_height[0] = lev_tree[0];
	}
	for (size_t i = 1; i < d; i++)
	{
		lev_tree[i] = lev_tree[0];
		if (mod_h == 1)
			lev_tree_height[i] = lev_tree[0];
	}
	// Заполняем строки нужными данными (0, -1 ОБЯЗАТЕЛЬНО!!!)
	fill_depth(tree, lev_tree, lev_tree_height, 0, -1, mod_h);
	// Удаляем лишнии столбцы
	delete_columns(lev_tree, lev_tree_height, skip, d, mod_h);
	// Печатаем дерево
	for (size_t i = 0; i < d; i++)
	{
		cout << "|" << lev_tree[i] << endl;
		if (mod_h == 1)
			cout << "|" << lev_tree_height[i] << endl;
	}

	std::cout << "\nTime: " << (float)(clock() - time_tree) / (float)1000000 << " c" << endl;
}

//=============================================================================================================================================================
//=============== РЕЖИМ ПЕЧАТИ ДЕРЕВА БЕЗ БУФЕРА (ДЕРЕВО НЕМНОГО БОЛЬШЕ ПО ШИРИНЕ, НО ПЕЧАТАЕТСЯ БЫСТРО, НЕТ РЕЖИМОВ ПЕЧАТИ ДОП ДАННЫХ)========================
//=============================================================================================================================================================

// Считает ширину элементов в графе, которые меньше заданного
// Что бы корректно считать отступы для одинаковых элементов, нужно знать номер слоя на котором находится элемент
int count_elem_min(node *tree, int a, int layer)
{
	if (tree == NULL)
		return (0);

	int result = 0;

	if (tree->info < a)
	{
		result += std::to_string(tree->info).length();
		result += count_elem_min(tree->l, a, layer);
		result += count_elem_min(tree->r, a, layer);
	}
	else if (tree->info == a && tree->layer > layer)
	{
		result += std::to_string(tree->info).length();
		result += count_elem_min(tree->l, a, layer);
	}
	else
		result += count_elem_min(tree->l, a, layer);

	return (result);
}

// Считает ширину элементов в графе, которые min <= заданный элемент <= max
// Что бы корректно считать отступы для одинаковых элементов, нужно знать номер слоя на котором находится элемент
// side = 0 для левого плеча, side = 1 для правого плеча
int count_elem_min_max(node *tree, int min, int max, int layer, int side)
{
	int result = 0;

	if (tree == NULL)
		return (0);

	if ((tree->info <= max && tree->info > min))
	{
		if (tree->info == max)
		{
			if (tree->layer > layer)
				result += std::to_string(tree->info).length();

			result += count_elem_min_max(tree->l, min, max, layer, side);
			result += count_elem_min_max(tree->r, min, max, layer, side);
		}
		else
		{
			result += std::to_string(tree->info).length();
			result += count_elem_min_max(tree->l, min, max, layer, side);
			result += count_elem_min_max(tree->r, min, max, layer, side);
		}
	}
	else
	{
		if (side == 0)
		{
			if (tree->l != NULL && tree->l->info != min)
				result += count_elem_min_max(tree->l, min, max, layer, side);
		}
		else
			result += count_elem_min_max(tree->l, min, max, layer, side);
		result += count_elem_min_max(tree->r, min, max, layer, side);
	}

	return (result);
}

// Печать нужного кол-ва символов
// Данный метод при тестировании оказался быстрее всего 
void	print_count_char(int num, char ch)
{
	string str_test;

	str_test.insert(0, num, ch);
	cout << str_test;
}

// Печать слоя дерева
void	print_layer(node *tree, int layer, node *tree_base, int *indent_prev, int *len_num_prev, bool *indent_prev_bool, int *right_arm_prev)
{
	if (tree == NULL)
		return ;

	int	indent = 0, temp = 0, left_arm = 0, right_arm = 0;

	if (tree->layer == layer)
	{
		// Вычисоение длины левого плеча
		if (tree->l != NULL)
			left_arm = count_elem_min_max(tree_base, tree->l->info, tree->info, tree->layer, 0) + to_string(tree->l->info).length();
		// Вычисоение правого левого плеча
		if (tree->r != NULL)
			right_arm = count_elem_min_max(tree_base, tree->info, tree->r->info, tree->layer, 1);

		indent = count_elem_min(tree_base, tree->info, tree->layer);
		temp = *indent_prev_bool == true ? temp = indent - *indent_prev - *len_num_prev : indent - *indent_prev;
		// Печать всего нужного на экран
		print_count_char(temp - left_arm - *right_arm_prev, ' ');
		print_count_char(left_arm, '_');
		cout << tree->info;
		print_count_char(right_arm, '_');
		// Замена старых данных на новые
		*indent_prev_bool = true;
		*len_num_prev = to_string(tree->info).length();
		*indent_prev = indent;
		*right_arm_prev = right_arm;
	}
	else if (tree->layer < layer)
	{
		print_layer(tree->l, layer, tree_base, indent_prev, len_num_prev, indent_prev_bool, right_arm_prev);
		print_layer(tree->r, layer, tree_base, indent_prev, len_num_prev, indent_prev_bool, right_arm_prev);
	}
}

// Печатает дерево по слоям
void print_tree_2(node *tree)
{
	int time_tree = clock(), d = depth(tree);

	int		indent_prev = 0;
	int		len_num_prev = 0;
	bool		indent_prev_bool = false;
	int		right_arm_prev = 0;

	cout << "Tree no buf\n" << "d= " << d << endl << "s= " << size(tree) << endl;

	// Печатаем слои
	for (size_t i = 0; i < d; i++)
	{
		cout << "|";
		print_layer(tree, i, tree, &indent_prev, &len_num_prev, &indent_prev_bool, &right_arm_prev);
		cout << endl;

		indent_prev = 0;
		len_num_prev = 0;
		right_arm_prev = 0;
		indent_prev_bool = false;
	}

	std::cout << "\nTime: " << (float)(clock() - time_tree) / (float)1000000 << " c" << endl;
}

//=============================================================================================================================================================
//================================================================ БЛОК С ТЕСТОМ ==============================================================================
//=============================================================================================================================================================

int	main ()
{
	node	*tree = NULL;
	string	str_test;
	int	rand_elem = 0, mod_tree = -1, temp_num = -1;

	srand(time(NULL));

	cout << "Введите 1, что бы смотреть как в дерево добавляются рандомные элементы" << endl;
	cout << "Введите 2, что бы самому добавлять элементы в дерево" << endl;
	cout << "Введите 3, что бы смотреть как в дерево добавляются рандомные элементы (под элементом печатается глубина дерева)" << endl;
	cout << "Введите 4, что бы самому добавлять элементы в дерево (под элементом печатается глубина дерева)" << endl;
	cout << "(В случае 3 и 4 используется более медленный алгоритм (во много раз медленнее!!!) (может отображать только числа от 0 до 999))" << endl;
	if (!getline(cin, str_test))
		return (1);
	if (str_test == "1" || str_test == "2" || str_test == "3" || str_test == "4")
		mod_tree = stoi(str_test);
	else
	{
		cout << "Неверный ввод" << endl;
		return (1);
	}

	for (size_t i = 0; i < 1000 ; i++)
	{
		clear();
		cout << "=======================" << endl;
		if (mod_tree == 3 || mod_tree == 4)
			print_tree(tree, 1);
		else
			print_tree_2(tree);
		if (mod_tree == 1 || mod_tree == 3)
		{
			rand_elem = rand() % 1000;
			push(rand_elem, &tree);
			cout << "Enter для добавления случайного элемента (введите exit для выхода)" << endl;
			if (!getline(cin, str_test))
				return (1);
			if (str_test == "exit")
				break ;
		}
		else
		{
			cout << "Введите значение элемента (от 0 до 999) (введите exit для выхода)" << endl;
			if (!getline(cin, str_test))
				return (1);
			if (str_test == "exit" || str_test == "")
				break ;
			temp_num = atoi(str_test.c_str());
			if ((temp_num < 0 || temp_num > 999) && mod_tree == 4)
				return (1);
			else
				push(atoi(str_test.c_str()), &tree);
		}
	}

	return (0);
}
