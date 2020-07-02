#include <iostream>
#include <fstream>
#include <string>
#include <functional>
using namespace std;
#define max(a, b) (a > b? a : b)

template<typename T, typename R = T>
class AVLBST {
	struct Node {
		T       element;
		Node*   left;
		Node*   right;
		int     height;
		int		row;

		Node(T element, int row) : row(row), element(element), 
			height(0), left(nullptr), right(nullptr) {}
	};
	Node*           root;
	int             len;
public:
	AVLBST() : root(nullptr), len(0) {}
	~AVLBST() { clear(root); }

	int size() { return len; }

	int height() { return height(root); }
	int height(Node* node) {
		return node == nullptr ? -1 : node->height;
	}

	void clear() { clear(root); len = 0; }
	void clear(Node*& node) {
		if (node != nullptr) {
			clear(node->left);
			clear(node->right);
			delete node;
			node = nullptr;
		}
	}

	void add(T elem, int row) { add(root, elem, row); }
	void add(Node*& node, T elem, int row) {
		if (node == nullptr) {
			node = new Node(elem, row);
			++len;
		}
		else {
			if (elem < node->element) {
				add(node->left, elem, row);
			}
			else {
				add(node->right, elem, row);
			}
			balance(node);
		}
	}

	bool find(R val) { return find(root, val); }
	bool find(Node* node, R val) {
		if (node == nullptr) {
			return false;
		}
		else if (val == node->element) {
			return true;
		}
		else if (val < node->element) {
			return find(node->left, val);
		}
		else {
			return find(node->right, val);
		}
	}

	void filter(int* filt, int tp, R val) {
		filter(root, filt, tp, val);
	}
	void filter(Node* node, int* filt, int tp, R val) {
		if (node == nullptr) { return; }

		if (tp == 1) { //mayor que
			if (node->element > val) {
				filt[node->row] = 1;
				filter(node->right, filt, tp, val);
				if (node->left != nullptr && node->left->element > val) filter(node->left, filt, tp, val);
			}
			else {
				filter(node->right, filt, tp, val);
			}
		}

		if (tp == 2) { //menor que
			if (node->element < val) {
				filt[node->row] = 1;
				filter(node->left, filt, tp, val);
				if (node->right != nullptr && node->right->element < val) filter(node->right, filt, tp, val);
			}
			else {
				filter(node->right, filt, tp, val);
			}
		}

		if (tp == 3) { //igual que
			if (node->element == val) {
				filt[node->row] = 1;
				filter(node->left, filt, tp, val);
				filter(node->right, filt, tp, val);
			}
			else if (val < node->element) {
				filter(node->left, filt, tp, val);
			}
			else {
				filter(node->right, filt, tp, val);
			}
		}
	}

	int* inOrder(int used_nrows) {
		int *new_order = new int[used_nrows];
		int *i = new int; *i = 0;
		inOrder(root, new_order, i);
		return new_order;
	}
	int* inOrder(Node* node, int*order, int*i) {
		int *aux = new int; *aux = 1;
		if (node != nullptr) {

			inOrder(node->left, order, i);
			order[*i] = node->row;
			*i += 1;
			inOrder(node->right, order, i);
		}
		return aux;
	}

	int* inOrderI(int used_nrows) {
		int *new_order = new int[used_nrows];
		int *i = new int; *i = 0;
		inOrderI(root, new_order, i);
		return new_order;
	}
	int* inOrderI(Node* node, int*order, int*i) {
		int *aux = new int; *aux = 1;
		if (node != nullptr) {

			inOrder(node->right, order, i);
			order[*i] = node->row;
			*i += 1;
			inOrder(node->left, order, i);
		}
		return aux;
	}

	void    updateHeight(Node* node) {
		if (node != nullptr) {
			int hl = height(node->left);
			int hr = height(node->right);

			node->height = max(hl, hr) + 1;
		}
	}
	void    rotateLeft(Node*& node) {
		Node* aux = node->right;
		node->right = aux->left;
		updateHeight(node);
		aux->left = node;
		updateHeight(aux);
		node = aux;
	}
	void    rotateRight(Node*& node) {
		Node* aux = node->left;
		node->left = aux->right;
		updateHeight(node);
		aux->right = node;
		updateHeight(aux);
		node = aux;
	}
	void    balance(Node*& node) {
		int hl = height(node->left);
		int hr = height(node->right);

		if (hr - hl < -1) {
			hl = height(node->left->left);
			hr = height(node->left->right);
			if (hr > hl) {
				rotateLeft(node->left);
			}
			rotateRight(node);
		}
		else if (hr - hl > 1) {
			hl = height(node->right->left);
			hr = height(node->right->right);
			if (hl > hr) {
				rotateRight(node->right);
			}
			rotateLeft(node);
		}
		else {
			updateHeight(node);
		}
	}
};

class Column {
protected:
	int nrows;
	string name;
	int type;
	bool indexado;
public:
	Column(int n, string name, int tp) {
		nrows = n;
		this->name = name;
		type = tp;
		indexado = false;
	}
	~Column() {}

	string getName() { return name; }
	bool getIndexado() { return indexado; }
	int getType() { return type; }

	virtual void index_col(int n) {}
	virtual void addrows() {}
	virtual void printElem(int i) {}
	virtual void insertElem(int pos) {}
	virtual float getElemF(int i) { return 0; }
	virtual string getElemS(int i) { return ""; }
	virtual int* ascendingOrder(int used_nrows) { return 0; }
	virtual int* descendingOrder(int used_nrows) { return 0; }
	virtual void filterF(int* filt, int tp, float val) {}
	virtual void filterS(int* filt, int tp, string val) {}
	virtual void saveElem(int i, ofstream &arch) {}
};

class ColumnString : public Column { //tipo2
	string *rows;
	AVLBST<string>* avl;
public:
	ColumnString(int nrows, string name, int tp) : Column(nrows, name, tp) {
		rows = new string[nrows];
	}
	~ColumnString() { delete rows; avl->clear(); }

	void addrows() {
		string *aux = new string[nrows * 2];
		for (int i = 0; i < nrows * 2; i++)
		{
			if (i < nrows)aux[i] = rows[i];
			else aux[i] = "";
		}
		delete[] rows;
		rows = aux;
		nrows *= 2;
	}

	void index_col(int used_nrows) {
		indexado = true;
		avl = new AVLBST<string>();

		for (int i = 0; i < used_nrows; ++i) {
			avl->add(rows[i], i);
		}
	}

	void printElem(int i) {
		cout << rows[i];
	}
	string getElemS(int i) {
		return rows[i];
	}
	void insertElem(int pos) {
		cin >> rows[pos];
		if (indexado) {
			avl->add(rows[pos], pos);
		}
	}

	int* ascendingOrder(int used_nrows) {
		if (!indexado) {
			string *rows_aux = new string[used_nrows];
			int *new_order = new int[used_nrows];

			for (int i = 0; i < used_nrows; i++) {
				rows_aux[i] = rows[i];
				new_order[i] = i;
			}

			int i, j, keyAux;
			string key;
			for (i = 1; i < used_nrows; i++) {
				key = rows_aux[i];
				keyAux = new_order[i];
				j = i - 1;

				while (j >= 0 && rows_aux[j] > key) {
					rows_aux[j + 1] = rows_aux[j];
					new_order[j + 1] = new_order[j];
					j--;
				}
				rows_aux[j + 1] = key;
				new_order[j + 1] = keyAux;
			}
			return new_order;
		}
		else {
			return avl->inOrder(used_nrows);
		}
	}

	int* descendingOrder(int used_nrows) {
		if (!indexado) {
			string *rows_aux = new string[used_nrows];
			int *new_order = new int[used_nrows];

			for (int i = 0; i < used_nrows; i++) {
				rows_aux[i] = rows[i];
				new_order[i] = i;
			}

			int i, j, keyAux;
			string key;
			for (i = 1; i < used_nrows; i++) {
				key = rows_aux[i];
				keyAux = new_order[i];
				j = i - 1;

				while (j >= 0 && rows_aux[j] < key) {
					rows_aux[j + 1] = rows_aux[j];
					new_order[j + 1] = new_order[j];
					j--;
				}
				rows_aux[j + 1] = key;
				new_order[j + 1] = keyAux;
			}
			return new_order;
		}
		else {
			return avl->inOrderI(used_nrows);
		}
	}

	void filterS(int* filt, int tp, string val) {
		avl->filter(filt, tp, val);
	}

	void saveElem(int i, ofstream &arch) {
		arch << rows[i];
	}
};

class ColumnFloat : public Column { //tipo 1
	float *rows;
	AVLBST<float>* avl;
public:
	ColumnFloat(int nrows, string name, int tp) : Column(nrows, name, tp) {
		rows = new float[nrows];
	}
	~ColumnFloat() { delete rows; avl->clear(); }

	void addrows() {
		float *aux = new float[nrows * 2];
		for (int i = 0; i < nrows * 2; i++)
		{
			if (i < nrows)aux[i] = rows[i];
			else aux[i] = -0.001;
		}
		delete[] rows;
		rows = aux;
		nrows *= 2;
	}

	void index_col(int used_nrows) {
		indexado = true;
		avl = new AVLBST<float>();

		for (int i = 0; i < used_nrows; ++i) {
			avl->add(rows[i], i);
		}
	}

	void printElem(int i) {
		cout << rows[i];		
	}
	float getElemF(int i) {
		return rows[i];
	}

	void insertElem(int pos) {		
		cin >> rows[pos];
		if (indexado) {
			avl->add(rows[pos], pos);
		}
	}

	int* ascendingOrder(int used_nrows) {
		if (!indexado) {
			float *rows_aux = new float[used_nrows];
			int *new_order = new int[used_nrows];

			for (int i = 0; i < used_nrows; i++) {
				rows_aux[i] = rows[i];
				new_order[i] = i;
			}

			int i, j, keyAux;
			float key;
			for (i = 1; i < used_nrows; i++) {
				key = rows_aux[i];
				keyAux = new_order[i];
				j = i - 1;

				while (j >= 0 && rows_aux[j] > key) {
					rows_aux[j + 1] = rows_aux[j];
					new_order[j + 1] = new_order[j];
					j--;
				}
				rows_aux[j + 1] = key;
				new_order[j + 1] = keyAux;
			}
			return new_order;
		}
		else {
			return avl->inOrder(used_nrows);
		}
	}

	int* descendingOrder(int used_nrows) {
		if (!indexado) {
			float *rows_aux = new float[used_nrows];
			int *new_order = new int[used_nrows];

			for (int i = 0; i < used_nrows; i++) {
				rows_aux[i] = rows[i];
				new_order[i] = i;
			}

			int i, j, keyAux;
			float key;
			for (i = 1; i < used_nrows; i++) {
				key = rows_aux[i];
				keyAux = new_order[i];
				j = i - 1;

				while (j >= 0 && rows_aux[j] < key) {
					rows_aux[j + 1] = rows_aux[j];
					new_order[j + 1] = new_order[j];
					j--;
				}
				rows_aux[j + 1] = key;
				new_order[j + 1] = keyAux;
			}
			return new_order;
		}
		else {
			return avl->inOrderI(used_nrows);
		}
	}

	void filterF(int* filt, int tp, float val) {
		avl->filter(filt, tp, val);
	}

	void saveElem(int i, ofstream &arch) {
		arch << rows[i];
	}
};

class Table {
	Column **columns;
	int ncolumns, maxrows, nrows;
public:
	//Creación de Tablas
	Table(int ncolumns, int nrows) {
		this->ncolumns = ncolumns;
		this->maxrows = nrows;
		this->nrows = 0;
		columns = new Column*[ncolumns];
	}
	~Table() {
		for (int i = 0; i < ncolumns; i++)
		{
			delete columns[i];
		}
		delete[] columns;
	}

	int getType_col(int col) { return columns[col]->getType(); }
	bool is_column_index(int col) { return columns[col]->getIndexado(); }
	int getNcolumns() { return ncolumns; }
	int getMaxrows() { return maxrows; }

	void printTable() {
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
		}
		cout << endl;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncolumns; j++) {
				columns[j]->printElem(i); cout << "\t\t";
			}
			cout << endl;
		}
	}

	//Inserción de columnas
	void insertColumn(int i, string cName, int type) {
		if (type == 1) columns[i] = new ColumnFloat(maxrows, cName, type);
		if (type == 2) columns[i] = new ColumnString(maxrows, cName, type);
	}

	//Inserción de datos
	void insertElement(int column, int row) {
		while (row >= maxrows) {
			for (int j = 0; j < ncolumns; j++) {
				columns[j]->addrows();
			}
			maxrows *= 2;
		}
		if (row >= nrows) nrows = row + 1;

		cout << "\nInserta elemento en Columna " << columns[column]->getName() << " Fila " << row << ": ";
		columns[column]->insertElem(row);
	}

	//Indexado de datos por columnas
	void index_c(int ncol) {
		columns[ncol]->index_col(nrows);
	}

	//Selección de datos por columnas.
	Table* seleccion(int scolumns, int *col) {
		Table* s_tabla = new Table(scolumns, maxrows);
		for (int i = 0; i < ncolumns; i++)
		{
			for (int j = 0; j < scolumns; j++)
			{
				if (col[j] == i) s_tabla->addColumn(j, columns[i]);
			}
		}
		s_tabla->set_nrows(nrows);
		return  s_tabla;
	}
	void set_nrows(int n) {
		nrows = n;
	}
	void addColumn(int i, Column* col) {
		columns[i] = col;
	}

	//Ordenamiento de datos por columnas
	void ordenamiento_as(int n_col, string nombre) {
		ofstream arch(nombre, ios::out);
		int *orden = columns[n_col]->ascendingOrder(nrows);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncolumns; j++) {
				columns[j]->printElem(orden[i]); cout << "\t\t";
				columns[j]->saveElem(orden[i], arch); arch << ",";
			}
			cout << endl;
			arch << endl;
		}
		arch.close();
	}
	void ordenamiento_des(int n_col, string nombre) {
		ofstream arch(nombre, ios::out);
		int *orden = columns[n_col]->descendingOrder(nrows);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncolumns; j++) {
				columns[j]->printElem(orden[i]); cout << "\t\t";
				columns[j]->saveElem(orden[i], arch); arch << ",";
			}
			cout << endl;
			arch << endl;
		}
		arch.close();
	}

	//filtro indexado
	void filtroI(int col, int tp, float val) {
		int *filt = new int[maxrows];
		for (int i = 0; i < maxrows; i++)
		{
			filt[i] = 0;
		}

		columns[col]->filterF(filt, tp, val);

		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (filt[i]) {
				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtroI(int col, int tp, string val) {
		int *filt = new int[nrows];
		filt = { 0 };
		columns[col]->filterS(filt, tp, val);

		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (filt[i]) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	//filtro indexado doble
	void filtroI2(int col, int tp, float val, int col2, int tp2, float val2) {
		int *filt = new int[maxrows];
		int *filt2 = new int[maxrows];
		for (int i = 0; i < maxrows; i++)
		{
			filt[i] = 0; filt2[i] = 0;
		}
		columns[col]->filterF(filt, tp, val);
		columns[col2]->filterF(filt2, tp2, val2);
		
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (filt[i] && filt2[i]) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtroI2(int col, int tp, float val, int col2, int tp2, string val2) {
		int *filt = new int[maxrows];
		int *filt2 = new int[maxrows];
		for (int i = 0; i < maxrows; i++)
		{
			filt[i] = 0; filt2[i] = 0;
		}
		columns[col]->filterF(filt, tp, val);
		columns[col2]->filterS(filt2, tp2, val2);

		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (filt[i] && filt2[i]) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtroI2(int col, int tp, string val, int col2, int tp2, string val2) {
		int *filt = new int[maxrows];
		int *filt2 = new int[maxrows];
		for (int i = 0; i < maxrows; i++)
		{
			filt[i] = 0; filt2[i] = 0;
		}
		columns[col]->filterS(filt, tp, val);
		columns[col2]->filterS(filt2, tp2, val2);

		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (filt[i] && filt2[i]) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}

	//filtro normal
	void filtro(function<bool(float n, float a)> key, int col, float n) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;
		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemF(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro(function<bool(char n, string a)> key, int col, char x) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(x, columns[col]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro(function<bool(string a, string b)> key, int col, string x) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(x, columns[col]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	//filtro normal doble
	void filtro2(function<bool(float n, float a)> key, int col, float n, function<bool(float n, float a)> key2, int col2, float n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemF(i)) && key2(n2, columns[col2]->getElemF(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro2(function<bool(float n, float a)> key, int col, float n, function<bool(char n, string a)> key2, int col2, char n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemF(i)) && key2(n2, columns[col2]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro2(function<bool(float n, float a)> key, int col, float n, function<bool(string n, string a)> key2, int col2, string n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemF(i)) && key2(n2, columns[col2]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro2(function<bool(char n, string a)> key, int col, char n, function<bool(char n, string a)> key2, int col2, char n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemS(i)) && key2(n2, columns[col2]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro2(function<bool(char n, string a)> key, int col, char n, function<bool(string n, string a)> key2, int col2, string n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemS(i)) && key2(n2, columns[col2]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}
	void filtro2(function<bool(string n, string a)> key, int col, string n, function<bool(string n, string a)> key2, int col2, string n2) {
		string nombre = columns[col]->getName();
		nombre += ".csv";
		ofstream arch(nombre, ios::out);

		for (int j = 0; j < ncolumns; j++) {
			cout << columns[j]->getName() << "\t\t";
			arch << columns[j]->getName() << ",";
		}
		cout << endl;
		arch << endl;

		for (int i = 0; i < nrows; i++) {

			if (key(n, columns[col]->getElemS(i)) && key2(n2, columns[col2]->getElemS(i))) {

				for (int j = 0; j < ncolumns; j++) {
					columns[j]->printElem(i); cout << "\t\t";
					columns[j]->saveElem(i, arch); arch << ",";
				}
				cout << endl;
				arch << endl;
			}
		}
		arch.close();
	}

	//Exportación de datos a archivos
	void save(string nombre) {		
		ofstream arch(nombre, ios::out);
		for (int j = 0; j < ncolumns; j++) {
			arch << columns[j]->getName() << ",";
			
		}
		arch << endl;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncolumns; j++) {
				columns[j]->saveElem(i, arch); arch << ",";
			}
			arch << endl;
		}
		arch.close();
	}
};


//filtros para parametros
bool greaterThan_1(float n, float a) {
	return a > n;
}
bool lesserThan_2(float n, float a) {
	return a < n;
}
bool equalTo_3(float n, float a) {
	return a == n;
}

bool startWith_4(char n, string a) {
	return n == a[0];
}
bool endsWith_5(char n, string a) {
	return n == a.back();
}

bool containsString_6(string q, string a) {
	return (a.find(q) != string::npos);
}
bool notcontainsString_7(string q, string a) {
	return (a.find(q) == string::npos);
}

bool greaterString_1(string b, string a) {

	return a > b;
}
bool lesserString_2(string b, string a) {

	return a < b;
}
bool equalString_3(string n, string a) {
	return a == n;
}

void MenuPrincipal(int t) {
	cout << endl;
	cout << "OPERACIONES DEL MINI-SGDB " << endl;
	cout << "(Trabajando con Tabla " << t << " de 9)" << endl;
	cout << "0.Elegir Tabla para trabajar " << endl;
	cout << "1.Creacion  de Tablas " << endl;
	cout << "2.Insercion de datos " << endl;
	cout << "3.Indexado  de datos por columnas " << endl;
	cout << "4.Seleccion de datos por columnas " << endl;
	cout << "5.Filtrado  de datos por columnas " << endl;
	cout << "6.Ordenamiento de datos por columnas " << endl;
	cout << "7.Exportacion  de datos a archivos planos " << endl;
	cout << "8.Visualizar Tabla actual " << endl;
	cout << "9.Limpiar Pantalla " << endl;
	cout << "10.Salir del programa " << endl << endl;
	cout << "Escoga una opcion: ";
}

int main() {
	int o, t=0;
	Table** tablas = new Table*[10];
	for (int i = 0; i < 10; i++)	
		tablas[i] = nullptr;	

	while (true)
	{
		MenuPrincipal(t);
		cin >> o;
		cout << endl;
		switch (o)
		{
		case 0:
			do
			{
				cout << "Elegir un numero de Tabla valido: ";
				cin >> t;
				cout << endl;
			} while (t < 0 || t > 9);

			break;
		case 1:
			if (tablas[t] != nullptr) {
				cout << "ATENCION!!! Ya se creo una tabla " << t << endl;
			}
			else {
				cout << "Crear Tabla: " << t << endl;
				int filas, columnas;
				int *tipos;
				string *columnasNombres;
				do
				{
					cout << "Inserta estimacion de cantidad de filas: ";
					cin >> filas;
				} while (filas<0);
				do
				{
					cout << "Inserta cantidad de columnas: ";
					cin >> columnas;
				} while (columnas < 1);				

				tipos = new int[columnas];
				columnasNombres = new string[columnas];
				for (int i = 0; i < columnas; i++) {
					cout << "Ingresa el título de la columna " << i  << ":" << endl;
					cin >> columnasNombres[i];
					do
					{
						cout << "Ingresa tipo de dato para la columna " << i  << " (1:Numero, 2: Texto)" << endl;
						cin >> tipos[i];
					} while (tipos[i] != 1 && tipos[i] != 2);
					
				}
				tablas[t] = new Table(columnas, filas);
				for (int i = 0; i < columnas; i++) {
					tablas[t]->insertColumn(i, columnasNombres[i], tipos[i]);
				}					
			}

			break;
		case 2:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {
				cout << "Insertar datos en Tabla: " << t << endl;
				int s;
				int c = tablas[t]->getNcolumns();
				cout << "Utilizando la estimacion de cantidad de filas, " << endl;
				cout << "Desea llenar todos los datos de la tabla? (No:0, Si:1): "; cin >> s;
				if (s) {
					int f = tablas[t]->getMaxrows();
					for (int i = 0; i < f; i++) {
						for (int j = 0; j < c; j++) {
							tablas[t]->insertElement(j, i);
						}
					}
				}
				else {
					int c2, f2;
					cout << "Selecciona donde insertar dato" << endl;
					do
					{
						cout << "Inserta numero de columna: ";
						cin >> c2;
					} while (c2 >= c);
					cout << "Inserta numero de fila: ";
					cin >> f2;
					tablas[t]->insertElement(c2, f2);
				}
				cout << endl;
				
			}
		}

			break;
		case 3:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {
				cout << "Indexar columna en Tabla " << t <<  endl;
				int c = tablas[t]->getNcolumns(), c2;
				do
				{
					cout << "Inserta numero de columna: ";
					cin >> c2;
				} while (c2 >= c);

				if (tablas[t]->is_column_index(c2))
					cout << "ATENCION!!! Columna ya esta indexada" << endl;
				else {
					tablas[t]->index_c(c2);
					cout << "Columna indexada exitosamente" << endl;
				}
			}
		}

		break;
		case 4:
		{
			bool espacio = false;
			for (int i = 0; i < 10; i++)
			{
				if (tablas[i] == nullptr) espacio = true;
			}

			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else if (!espacio) {
				cout << "ATENCION!!! No hay espacio para almacenar la nueva tabla que esta funcion generaria" << endl;
			}
			else {
				cout << "Seleccionar columna(s) " << endl;
				cout << "La Nueva Tabla se almacenara en uno de los espacios actuales de trabajo" << endl;
				cout << "Para visualizar la Nueva Tabla, debera elegirla para trabajar " << endl;
				int nseleccion = 0;
				int* a_sel = new int[nseleccion];				
				int aux;
				while (true)
				{
					int c = tablas[t]->getNcolumns(), c2;
					do
					{
						cout << endl << "Selecciona numero de columna (-1 para terminar) :";
						cin >> c2;
					} while (c2 >= c);
					
					aux = c2;

					if (aux != -1) { a_sel[nseleccion] = aux; nseleccion++; }
					else break;
				}

				int auxT;
				do
				{
					cout << "Inserta espacio vacio donde almacenar nueva Tabla (0-9): ";
					cin >> auxT;
				} while (tablas[auxT] != nullptr);		

				tablas[auxT] = tablas[t]->seleccion(nseleccion, a_sel);
				
			}
				
		}

		break;
		case 5:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {
				cout << "Filtrar columna(s) en tabla" << t << endl;
				cout << "La Nueva Tabla se mostrara en pantalla y se almacenara en un archivo" << endl;
				int n_filtros;
				do
				{
					cout << "Cuantos filtros va a aplicar (1 o 2)?" << endl;
					cin >> n_filtros;
				} while (n_filtros !=1 && n_filtros != 2);

				if (n_filtros == 1) {
					int c_max = tablas[t]->getNcolumns(), c1;
					do
					{
						cout << "Inserta numero de columna a filtrar: ";
						cin >> c1;
					} while (c1 >= c_max);
					int tp_col = tablas[t]->getType_col(c1);

					if (tp_col == 1) {
						int filtro;
						float x;
						cout << "ELEGIR FILTRO:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro;
						} while (filtro < 1 || filtro > 3);
						cout << "Inserta parametro para filtrar: ";
						cin >> x;

						if (tablas[t]->is_column_index(c1)) {
							tablas[t]->filtroI(c1, filtro, x);							
						}
						else {
							if (filtro == 1) tablas[t]->filtro(greaterThan_1, c1, x);
							if (filtro == 2) tablas[t]->filtro(lesserThan_2, c1, x);
							if (filtro == 3) tablas[t]->filtro(equalTo_3, c1, x);
						}
						cout << endl;
					}
					else {
						int filtro;
						string x;
						char y;
						cout << "ELEGIR FILTRO:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						cout << "4.Inicia con " << endl;
						cout << "5.Finaliza con " << endl;
						cout << "6.Esta contenido en " << endl;
						cout << "7.No esta contenido en " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro;
						} while (filtro < 1 || filtro > 7);
						cout << "Inserta parametro para filtrar: ";
						if (filtro == 4 || filtro == 5) cin >> y;
						else cin >> x;

						if (tablas[t]->is_column_index(c1) && (filtro == 1 || filtro == 2 || filtro == 3)) {
							tablas[t]->filtroI(c1, filtro, x);
						}
						else {
							if (filtro == 1) tablas[t]->filtro(greaterString_1, c1, x);
							if (filtro == 2) tablas[t]->filtro(lesserString_2, c1, x);
							if (filtro == 3) tablas[t]->filtro(equalString_3, c1, x);
							if (filtro == 4) tablas[t]->filtro(startWith_4, c1, y);
							if (filtro == 5) tablas[t]->filtro(endsWith_5, c1, y);
							if (filtro == 6) tablas[t]->filtro(containsString_6, c1, x);
							if (filtro == 7) tablas[t]->filtro(notcontainsString_7, c1, x);
						}					
					}
				}
				else {//dos filtros
					int c_max = tablas[t]->getNcolumns(), c1,c2;
					do
					{
						cout << "Recordatorio: Las 2 columnas deben estar indexadas o las 2 deben estar no indexadas" << endl;
						cout << "No podras elegir una indexada y otra no indexada" << endl;
						do
						{
							cout << "Inserta numero de columna 1 a filtrar: ";
							cin >> c1;
						} while (c1 >= c_max);
						do
						{
							cout << "Inserta numero de columna 2 a filtrar: ";
							cin >> c2;
						} while (c2 >= c_max);

					} while ((tablas[t]->is_column_index(c1) != tablas[t]->is_column_index(c2)));
					

					int tp_col = tablas[t]->getType_col(c1);
					int tp_col2 = tablas[t]->getType_col(c2);

					int filtro, filtro2;

					float n, n2;
					string x, x2;
					char y, y2;

					if (tp_col == 1) {						
						cout << "ELEGIR FILTRO 1:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro;
						} while (filtro < 1 || filtro > 3);
						cout << "Inserta parametro 1 para filtrar: ";
						cin >> n;
					}
					else {						
						cout << "ELEGIR FILTRO 1:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						cout << "4.Inicia con " << endl;
						cout << "5.Finaliza con " << endl;
						cout << "6.Esta contenido en " << endl;
						cout << "7.No esta contenido en " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro;
						} while (filtro < 1 || filtro > 7);
						cout << "Inserta parametro 1 para filtrar: ";
						if (filtro == 4 || filtro == 5) cin >> y;
						else cin >> x;						
					}

					if (tp_col2 == 1) {
						cout << "ELEGIR FILTRO 2:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro2;
						} while (filtro2 < 1 || filtro2 > 3);
						cout << "Inserta parametro 2 para filtrar: ";
						cin >> n2;
					}
					else {
						cout << "ELEGIR FILTRO 2:" << endl;
						cout << "1.Mayor " << endl;
						cout << "2.Menor " << endl;
						cout << "3.Igual a " << endl;
						cout << "4.Inicia con " << endl;
						cout << "5.Finaliza con " << endl;
						cout << "6.Esta contenido en " << endl;
						cout << "7.No esta contenido en " << endl;
						do
						{
							cout << "Escoga una opcion: ";
							cin >> filtro2;
						} while (filtro2 < 1 || filtro2 > 7);
						cout << "Inserta parametro 2 para filtrar: ";
						if (filtro2 == 4 || filtro2 == 5) cin >> y2;
						else cin >> x2;
					}

					cout << endl;
					if (tablas[t]->is_column_index(c1) && (filtro == 1 || filtro == 2 || filtro == 3) && (filtro2 == 1 || filtro2 == 2 || filtro2 == 3)) {
						if (tp_col == 1 && tp_col2 == 1) tablas[t]->filtroI2(c1, filtro, n, c2, filtro2, n2);
						if (tp_col == 1 && tp_col2 == 2) tablas[t]->filtroI2(c1, filtro, n, c2, filtro2, x2);
						if (tp_col == 2 && tp_col2 == 2) tablas[t]->filtroI2(c1, filtro, x, c2, filtro2, x2);
						if (tp_col == 2 && tp_col2 == 1) tablas[t]->filtroI2(c2, filtro2, n2, c1, filtro, x);
					}
					else {
						if (filtro == 1) {
							if (tp_col == 1) {
								if (filtro2 == 1 && tp_col2 == 1) tablas[t]->filtro2(greaterThan_1, c1, n, greaterThan_1, c2, n2);
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(greaterThan_1, c1, n, greaterString_1, c2, x2);
								if (filtro2 == 2 && tp_col2 == 1) tablas[t]->filtro2(greaterThan_1, c1, n, lesserThan_2, c2, n2);
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(greaterThan_1, c1, n, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 1) tablas[t]->filtro2(greaterThan_1, c1, n, equalTo_3, c2, n2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(greaterThan_1, c1, n, equalString_3, c2, x2);
								if (filtro2 == 4)	tablas[t]->filtro2(greaterThan_1, c1, n, startWith_4, c2, y2);
								if (filtro2 == 5)	tablas[t]->filtro2(greaterThan_1, c1, n, endsWith_5, c2, y2);
								if (filtro2 == 6)	tablas[t]->filtro2(greaterThan_1, c1, n, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(greaterThan_1, c1, n, notcontainsString_7, c2, x2);
							}
							if (tp_col == 2) {
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(greaterString_1, c1, x, greaterString_1, c2, x2);								
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(greaterString_1, c1, x, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(greaterString_1, c1, x, equalString_3, c2, x2);
								if (filtro2 == 6)	tablas[t]->filtro2(greaterString_1, c1, x, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(greaterString_1, c1, x, notcontainsString_7, c2, x2);
							}
						}
						if (filtro == 2) {
							if (tp_col == 1) {
								if (filtro2 == 1 && tp_col2 == 1) tablas[t]->filtro2(lesserThan_2, c1, n, greaterThan_1, c2, n2);
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(lesserThan_2, c1, n, greaterString_1, c2, x2);
								if (filtro2 == 2 && tp_col2 == 1) tablas[t]->filtro2(lesserThan_2, c1, n, lesserThan_2, c2, n2);
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(lesserThan_2, c1, n, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 1) tablas[t]->filtro2(lesserThan_2, c1, n, equalTo_3, c2, n2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(lesserThan_2, c1, n, equalString_3, c2, x2);
								if (filtro2 == 4)	tablas[t]->filtro2(lesserThan_2, c1, n, startWith_4, c2, y2);
								if (filtro2 == 5)	tablas[t]->filtro2(lesserThan_2, c1, n, endsWith_5, c2, y2);
								if (filtro2 == 6)	tablas[t]->filtro2(lesserThan_2, c1, n, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(lesserThan_2, c1, n, notcontainsString_7, c2, x2);
							}
							if (tp_col == 2) {
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(lesserString_2, c1, x, greaterString_1, c2, x2);
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(lesserString_2, c1, x, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(lesserString_2, c1, x, equalString_3, c2, x2);
								if (filtro2 == 6)	tablas[t]->filtro2(lesserString_2, c1, x, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(lesserString_2, c1, x, notcontainsString_7, c2, x2);
							}
						}
						if (filtro == 3) {
							if (tp_col == 1) {
								if (filtro2 == 1 && tp_col2 == 1) tablas[t]->filtro2(equalTo_3, c1, n, greaterThan_1, c2, n2);
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(equalTo_3, c1, n, greaterString_1, c2, x2);
								if (filtro2 == 2 && tp_col2 == 1) tablas[t]->filtro2(equalTo_3, c1, n, lesserThan_2, c2, n2);
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(equalTo_3, c1, n, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 1) tablas[t]->filtro2(equalTo_3, c1, n, equalTo_3, c2, n2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(equalTo_3, c1, n, equalString_3, c2, x2);
								if (filtro2 == 4)	tablas[t]->filtro2(equalTo_3, c1, n, startWith_4, c2, y2);
								if (filtro2 == 5)	tablas[t]->filtro2(equalTo_3, c1, n, endsWith_5, c2, y2);
								if (filtro2 == 6)	tablas[t]->filtro2(equalTo_3, c1, n, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(equalTo_3, c1, n, notcontainsString_7, c2, x2);
							}
							if (tp_col == 2) {
								if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(equalString_3, c1, x, greaterString_1, c2, x2);
								if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(equalString_3, c1, x, lesserString_2, c2, x2);
								if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(equalString_3, c1, x, equalString_3, c2, x2);
								if (filtro2 == 6)	tablas[t]->filtro2(equalString_3, c1, x, containsString_6, c2, x2);
								if (filtro2 == 7)	tablas[t]->filtro2(equalString_3, c1, x, notcontainsString_7, c2, x2);
							}
						}
						if (filtro == 4) {							
							if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(startWith_4, c1, y, greaterString_1, c2, x2);
							if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(startWith_4, c1, y, lesserString_2, c2, x2);
							if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(startWith_4, c1, y, equalString_3, c2, x2);
							if (filtro2 == 4)	tablas[t]->filtro2(startWith_4, c1, y, startWith_4, c2, y2);
							if (filtro2 == 5)	tablas[t]->filtro2(startWith_4, c1, y, endsWith_5, c2, y2);
							if (filtro2 == 6)	tablas[t]->filtro2(startWith_4, c1, y, containsString_6, c2, x2);
							if (filtro2 == 7)	tablas[t]->filtro2(startWith_4, c1, y, notcontainsString_7, c2, x2);
								
						}
						if (filtro == 5) {
							if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(endsWith_5, c1, y, greaterString_1, c2, x2);
							if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(endsWith_5, c1, y, lesserString_2, c2, x2);
							if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(endsWith_5, c1, y, equalString_3, c2, x2);
							if (filtro2 == 4)	tablas[t]->filtro2(endsWith_5, c1, y, startWith_4, c2, y2);
							if (filtro2 == 5)	tablas[t]->filtro2(endsWith_5, c1, y, endsWith_5, c2, y2);
							if (filtro2 == 6)	tablas[t]->filtro2(endsWith_5, c1, y, containsString_6, c2, x2);
							if (filtro2 == 7)	tablas[t]->filtro2(endsWith_5, c1, y, notcontainsString_7, c2, x2);

						}
						if (filtro == 6) {
							if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(containsString_6, c1, x, greaterString_1, c2, x2);
							if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(containsString_6, c1, x, lesserString_2, c2, x2);
							if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(containsString_6, c1, x, equalString_3, c2, x2);
							if (filtro2 == 6)	tablas[t]->filtro2(containsString_6, c1, x, containsString_6, c2, x2);
							if (filtro2 == 7)	tablas[t]->filtro2(containsString_6, c1, x, notcontainsString_7, c2, x2);	
						}
						if (filtro == 7) {
							if (filtro2 == 1 && tp_col2 == 2) tablas[t]->filtro2(notcontainsString_7, c1, x, greaterString_1, c2, x2);
							if (filtro2 == 2 && tp_col2 == 2) tablas[t]->filtro2(notcontainsString_7, c1, x, lesserString_2, c2, x2);
							if (filtro2 == 3 && tp_col2 == 2) tablas[t]->filtro2(notcontainsString_7, c1, x, equalString_3, c2, x2);
							if (filtro2 == 6)	tablas[t]->filtro2(notcontainsString_7, c1, x, containsString_6, c2, x2);
							if (filtro2 == 7)	tablas[t]->filtro2(notcontainsString_7, c1, x, notcontainsString_7, c2, x2);
						}
					}

				}

				cout <<endl << "Observacion!!! Si no se muestra una Tabla filtrada, los parametros ingresados fueron invalidos (vuelva a intentar)" << endl;				
			}
				
		}

		break;

		case 6:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {
				cout << "Ordenar por columna en tabla" << t <<  endl;
				cout << "La Nueva Tabla se mostrara en pantalla y se almacenara en un archivo" << endl;
				int c = tablas[t]->getNcolumns(), c2;
				do
				{
					cout << "Inserta numero de columna: ";
					cin >> c2;
				} while (c2 >= c);

				int ord;
				cout << "Ascendente (0) o Descendente(1): " << endl;
				cin >> ord;

				string nombre;
				cout << "Inserta nombre para la NUeva Tabla: " << endl;
				cin >> nombre;
				nombre += ".csv";

				if (ord) tablas[t]->ordenamiento_des(c2,nombre);
				else tablas[t]->ordenamiento_as(c2,nombre);

			}
				
		}

		break;
		case 7:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {
				string nombre;
				cout << "Ingrese nombre del archivo: ";
				cin >> nombre;
				nombre += ".csv";
				tablas[t]->save(nombre);
				cout << endl << "Archivo creado exitosamente" << endl;
			}				
		}

		break;
		case 8:
		{
			if (tablas[t] == nullptr) {
				cout << "ATENCION!!! No hay una tabla creada en " << t << endl;
			}
			else {				
				tablas[t]->printTable();
			}
		}

		break;
		case 9:
		{
			system("CLS");
		}

		break;
		case 10:
		{
			
			exit(0);
		}

		break;
		default:
			cout << "ATENCION!!! Opcion Invalida" << endl;
			break;
		}
	}	
	
	for (int i = 0; i < 10; i++)
	{
		delete tablas[i];
	}
	delete[] tablas;

	return 0;
}