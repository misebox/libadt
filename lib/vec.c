#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vec.h"

// vector contains generic pointer
pvec
vec_new() {
    pvec v = (pvec)calloc(1, sizeof(vec_t));
    if (v) {
        v->reserved = 8;
        v->items = (voidptr_t)calloc(v->reserved, sizeof(voidptr_t));
        v->length = 0;
    }
    return v;
}

void
vec_free(pvec *_v) {
    pvec v = *_v;
    if (v) {
        free(v->items);
        v->items = NULL;
    }
    free(v);
    *_v = (pvec)NULL;
}

bool
vec_reserve(pvec v, uint64_t reserve) {
    if (v->length > reserve)
        return false;
    if (v->reserved >= reserve)
        return true;
    voidptr_t *new_items = (voidptr_t )realloc(v->items, sizeof(voidptr_t ) * reserve);
    if (new_items==NULL)
        return false;
    v->items = new_items;
    v->reserved = reserve;
    if (v->reserved > v->length) {
        uint64_t diff = v->reserved - v->length;
        memset(v->items + v->length, 0, sizeof(voidptr_t) * diff);
    }
    return true;
}

bool
vec_push(pvec v, voidptr_t item) {
    return vec_insert(v, item, v->length);
}

bool
vec_insert(pvec v, voidptr_t item, uint64_t pos) {
    if (v->length < pos)
        return false;
    if (v->reserved == v->length) {
        uint64_t reserve = v->length * 2;
        if (!vec_reserve(v, reserve))
            return false;
    }
    for (uint64_t i = v->length; i > pos; i--)
        v->items[i] = v->items[i-1];

    v->items[pos] = item;
    v->length++;
    return true;
}

bool
vec_del(pvec v, uint64_t idx) {
    if (v->length <= idx)
        return false;

    for (uint64_t i = idx; i < v->length -1; i++)
        v->items[i] = v->items[i+1];
    v->items[v->length-1] = 0;
    v->length --;
    return true;
}

voidptr_t
vec_get(pvec v, uint64_t idx) {
    voidptr_t item = NULL;
    if (v->length > idx) {
        item = v->items[idx];
    }
    return item;
}

voidptr_t
vec_pop(pvec v) {
    if (v->length == 0)
        return NULL;
    uint64_t idx = v->length-1;
    voidptr_t item = vec_get(v, idx);
    if (item && !vec_del(v, idx))
        return NULL;
    return item;
}

pvec
vec_copy(pvec v) {
    return vec_copy_slice(v, 0, v->length);
}

pvec
vec_copy_slice(pvec v, uint64_t start, uint64_t end) {
    if (v->length <= start || v->length < end || start > end)
        return NULL;
    pvec v_dst = vec_new();
    if (!v_dst)
        return NULL;
    uint64_t dst_length = end - start;
    if (v_dst->reserved < dst_length)
        if (!vec_reserve(v_dst, dst_length))
            return NULL;
    for (uint64_t i = 0; i < dst_length; i++)
        v_dst->items[i] = v->items[start + i];
    v_dst->length = dst_length;
    return v_dst;
}

bool
vec_find_ptr(pvec v, uint64_t *idx, voidptr_t target) {
    for (uint64_t i = 0; i < v->length; i++)
        if (v->items[i] == target) {
            *idx = i;
            return true;
        }
    return false;
}
