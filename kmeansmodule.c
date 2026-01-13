#define PY_SSIZE_T_CLEAN

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Python.h>

static struct vector *global_vec_ptr = NULL;
static struct center *global_cent_ptr = NULL;
static int DIM = 0;

struct cord
{
    double value;
    struct cord *next;
};

struct vector
{
    struct vector *next;
    struct cord *cords;
};

struct center
{
    struct center *next;
    struct cord *cords;
    struct cord *sum_assigned;
    int num_assigned;
};

struct cord* create_cord_list_from_py(PyObject *);
struct cord* create_zero_cord_list(int);
struct vector* py_list_to_vector(PyObject *);
struct center* py_list_to_center(PyObject *);
static PyObject* fit(PyObject *, PyObject *);
int kmeans(int, double, struct vector*, struct center*);
void delete_point(struct cord*);
void delete_points_list(struct vector*);
void delete_centers_list(struct center*);
void free_all();
struct cord* create_cord_node(double, struct center*);
struct center *create_center(struct vector*);
double points_distance(struct cord*, struct cord*);
struct center *closest_center(struct vector*, struct center*);
void assign_vector_to_center(struct vector*, struct center*);
void update_all_points(struct vector*, struct center*);
void delete_point_from_its_center(struct center*, struct vector*);
int update_all_centers(struct center*, struct vector*);

struct cord* create_cord_list_from_py(PyObject *py_list) {
    struct cord *head = NULL, *curr = NULL;
    
    for (Py_ssize_t i = 0; i < DIM; i++) {
        double val = PyFloat_AsDouble(PyList_GetItem(py_list, i));
        struct cord *new_cord = malloc(sizeof(struct cord));
        if (new_cord == NULL) {
            printf("An Error Has Occurred\n");
            free_all();
            exit(1);   
        }
        new_cord->value = val;
        new_cord->next = NULL;

        if (head == NULL) {
            head = new_cord;
            curr = head;
        } else {
            curr->next = new_cord;
            curr = new_cord;
        }
    }
    return head;
}

struct cord* create_zero_cord_list() {
    struct cord *head = NULL, *curr = NULL;

    for (int i = 0; i < DIM; i++) {
        struct cord *new_cord = malloc(sizeof(struct cord));
        if (new_cord == NULL) {
            printf("An Error Has Occurred\n");
            free_all();
            exit(1);   
        }
        new_cord->value = 0.0;
        new_cord->next = NULL;

        if (head == NULL) {
            head = new_cord;
            curr = head;
        } else {
            curr->next = new_cord;
            curr = new_cord;
        }
    }
    return head;
}

struct vector* py_list_to_vector(PyObject *py_list) {
    struct vector *head = NULL, *curr = NULL;
    Py_ssize_t n = PyList_Size(py_list);
    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject *py_point = PyList_GetItem(py_list, i);
        struct vector *new_vec = malloc(sizeof(struct vector));
        if (new_vec == NULL) {
            printf("An Error Has Occurred\n");
            free_all();
            exit(1);
        }
        new_vec->cords = create_cord_list_from_py(py_point);
        new_vec->next = NULL;

        if (head == NULL) {
            head = new_vec;
            curr = head;
        } else {
            curr->next = new_vec;
            curr = new_vec;
        }

        return head;
}

struct center* py_list_to_center(PyObject *py_list) {
    struct center *head = NULL, *curr = NULL;
    Py_ssize_t k = PyList_Size(py_list);

    PyObject *first_point = PyList_GetItem(py_list, 0);
    int dim = (int)PyList_Size(first_point);

    for (Py_ssize_t i = 0; i < k; i++) {
        PyObject *py_center = PyList_GetItem(py_list, i);
        struct center *new_center = malloc(sizeof(struct center));
        if (new_center == NULL) {
            printf("An Error Has Occurred\n");
            free_all();
            exit(1);
        }
        new_center->cords = create_cord_list_from_py(py_center);
        new_center->sum_assigned = create_zero_cord_list(dim);;
        new_center->num_assigned = 0;
        new_center->next = NULL;

        if (head == NULL) {
            head = new_center;
            curr = head;
        } else {
            curr->next = new_center;
            curr = new_center;
        }
    }
    return head;
}

static PyObject* fit(PyObject *self, PyObject *args) {
    /*gets iter, eps, list of datapoints and list of centroides*/
    int iter;
    double eps;
    PyObject *py_points_list;
    PyObject *py_centers_list;

    if(!PyArg_ParseTuple(args, "idOO", &iter, &eps, &py_points_list, &py_centers_list)) {
        return NULL; /* Error in parsing arguments */
    }

    DIM = (int)PyList_Size(PyList_GetItem(py_points_list, 0));

    struct vector *c_points = py_list_to_vector(py_points_list);
    struct center *c_centers = py_list_to_center(py_centers_list);

    kmeans(iter, eps, c_points, c_centers);

    /* Convert Result back C -> Python */

    /* Clean up C memory before returning to Python */
     
    return Py_BuildValue("d", kmeans(z, n)); /*  Py_BuildValue(...) returns a PyObject*  */
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                 /* the Python method name that will be used */
      (PyCFunction) fit,    /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,         /* flags indicating parameters accepted for this function */
    /*probably need to change that one - i get more arguments */

      PyDoc_STR("calculates centroides from a list of datapoints and a list of init_centroides using kmeans")}, 
      /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

static struct PyModuleDef mykmeanssp = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",       /* name of module */
    NULL,               /* module documentation, may be NULL */
    -1,                 /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods       /* the PyMethodDef array from before containing the methods of the extension */
};


void kmeans(int iter, double eps, struct vector head_vec, struct center centers) /*change so that it gets python objects for the lists*/
{
    int count, changed;
    struct vector head_vec;
    struct center centers;
    
    global_vec_ptr = head_vec;
    global_cent_ptr = centers;

    count = 0;
    changed = 1;

    while (changed && (count < iter))
    {
        update_all_points(head_vec, centers);
        changed = update_all_centers(centers, head_vec);
        count++;
    }
}

static PyMethodDef kmeansMethods[] = {
    {"kmeans",                   /* the Python method name that will be used */
      (PyCFunction) kmeans, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
/*probably need to change that one - i get more arguments */
      PyDoc_STR("calculates centroides from a list of datapoints and a list of init_centroides using kmeans")}, 
      /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeans_capi", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

void delete_point(struct cord *curr)
{
    if (curr != NULL)
    {
        delete_point(curr->next);
        free(curr);
    }
}

void delete_points_list(struct vector *curr)
{
    if (curr != NULL)
    {
        delete_point(curr->cords);
        delete_points_list(curr->next);
        free(curr);
    }
}

void delete_centers_list(struct center *curr)
{
    if (curr != NULL)
    {
        delete_point(curr->cords);
        delete_point(curr->sum_assigned);
        delete_centers_list(curr->next);
        free(curr);
    }
}

void free_all()
{
    delete_points_list(global_vec_ptr);
    delete_centers_list(global_cent_ptr);
}

struct cord* create_cord_node(double val, struct center *cent) {
    struct cord *new_node = malloc(sizeof(struct cord));
    if (new_node == NULL) {
        printf("An Error Has Occurred\n");
        delete_centers_list(cent);
        free_all();
        exit(1);
    }
    new_node->value = val;
    new_node->next = NULL;
    return new_node;
}

struct center *create_center(struct vector *vec)
{
    struct center *new_center = malloc(sizeof(struct center));
    struct cord *curr_v, *last_center, *last_sum;
    struct cord *new_center_cord, *new_sum_cord;

    if (new_center == NULL) {
        printf("An Error Has Occurred\n");
        free_all();
        exit(1);
    }
    new_center->next = NULL;
    new_center->num_assigned = 0;
    
    new_center->cords = NULL;
    new_center->sum_assigned = NULL;

    curr_v = vec->cords;
    last_center = NULL;
    last_sum = NULL;

    while (curr_v != NULL) {
        new_center_cord = create_cord_node(curr_v->value, new_center);
        new_sum_cord = create_cord_node(0.0, new_center);

        if (new_center->cords == NULL) {
            new_center->cords = new_center_cord;
            new_center->sum_assigned = new_sum_cord;
        } else {
            last_center->next = new_center_cord;
            last_sum->next = new_sum_cord;
        }
        
        last_center = new_center_cord;
        last_sum = new_sum_cord;
        curr_v = curr_v->next;
    }

    return new_center;
}


double points_distance(struct cord *p1, struct cord *p2){
    double distance = 0.0;
    while (p1 != NULL && p2 != NULL)
    {
        distance += pow(p1->value - p2->value, 2);
        p1 = p1->next;
        p2 = p2->next;
    }

    return sqrt(distance);
}

struct center *closest_center(struct vector *vec, struct center *centers)
{
    struct center *curr_center = centers;
    struct center *closest = NULL;
    double min_distance = HUGE_VAL, distance;
    struct cord *vec_cord, *center_cord;

    while(curr_center != NULL)
    {
        vec_cord = vec->cords;
        center_cord = curr_center->cords;
        distance = points_distance(vec_cord, center_cord);

        if (distance < min_distance)
        {
            min_distance = distance;
            closest = curr_center;
        }
        curr_center = curr_center->next;
    }
    return closest;
}

void assign_vector_to_center(struct vector *vec, struct center *center)
{
    struct cord *vec_cord = vec->cords;
    struct cord *sum_cord = center->sum_assigned;
    while (vec_cord != NULL && sum_cord != NULL) {
        sum_cord->value += vec_cord->value;

        vec_cord = vec_cord->next;
        sum_cord = sum_cord->next;
    }
    center->num_assigned += 1;
}

void update_all_points(struct vector *vectors, struct center *centers)
{
    struct vector *curr_vec = vectors;
    struct center *closest;
    while (curr_vec != NULL && curr_vec->next != NULL)
    {
        closest = closest_center(curr_vec, centers);
        assign_vector_to_center(curr_vec, closest);
        curr_vec = curr_vec->next;
    }
}

void delete_point_from_its_center(struct center *centers, struct vector *point)
{
    struct center *closest = closest_center(point, centers);
    struct cord *p_curr = point->cords;
    struct cord *c_curr = closest->sum_assigned;
    closest->num_assigned--;
    while (p_curr != NULL && c_curr != NULL)
    {
        c_curr->value -= p_curr->value;

        p_curr = p_curr->next;
        c_curr = c_curr->next;
    }
}

int update_all_centers(struct center *centers, struct vector *first_point)
{
    int moved = 0;
    double eps = 0.001;
    struct center *curr_cent = centers;
    struct cord *tmp;

    while (curr_cent!=NULL)
    {
        if (curr_cent->num_assigned == 0){
            assign_vector_to_center(first_point, curr_cent);
            delete_point_from_its_center(centers, first_point);
        }
        curr_cent = curr_cent->next;
    }
    curr_cent = centers;
    while (curr_cent!=NULL)
    {
        /* Calculate the updated coaordinates, and the distance */
        struct cord *curr_cord = curr_cent->sum_assigned;
        while (curr_cord!=NULL)
        {
            curr_cord->value = curr_cord->value / curr_cent->num_assigned;
            curr_cord = curr_cord->next;
        }
        if (points_distance(curr_cent->cords, curr_cent->sum_assigned) >= eps)
        {
            moved = 1;
        }

        /* Update the center, and delete all assigned points from it */
        tmp = curr_cent->cords;
        curr_cent->cords = curr_cent->sum_assigned;
        curr_cord = tmp;
        while (curr_cord!=NULL)
        {
            curr_cord->value = 0;
            curr_cord = curr_cord->next;
        }
        curr_cent->sum_assigned = tmp;
        curr_cent->num_assigned = 0;
        curr_cent = curr_cent->next;
    }
    return moved;
}
