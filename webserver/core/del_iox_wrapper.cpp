/*
 * del_iox_wrapper.cpp - DEL thin wrapper around IceOryx2 C binding
 *
 * Part of the Data Exchange Layer (DEL) for OpenPLC v3.
 *
 * When _iceoryx2_src is defined, this file uses the real IceOryx2 C FFI.
 * Otherwise it compiles as a no-op stub so the build succeeds without
 * IceOryx2 installed.
 *
 * Copyright (C) 2025 OpenPLC DEL Contributors
 * Licensed under GPL v3 (same as OpenPLC v3)
 */

#include "del_iox_wrapper.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _iceoryx2_src

/* ================================================================== */
/*  Real implementation using IceOryx2 C binding                       */
/* ================================================================== */

#include "iox2/iceoryx2.h"

/* --- Internal structures holding IceOryx2 handles --- */

struct del_node_s {
    iox2_node_h handle;
};

struct del_publisher_s {
    iox2_publisher_h   handle;
    size_t             payload_size;
};

struct del_subscriber_s {
    iox2_subscriber_h  handle;
    size_t             payload_size;
};

/* --- Node --- */

del_node_t del_node_create(const char *name) {
    del_node_t n = (del_node_t)calloc(1, sizeof(struct del_node_s));
    if (!n) return NULL;

    iox2_node_builder_h builder = iox2_node_builder_new(NULL);
    if (!builder) {
        free(n);
        return NULL;
    }

    /* Set node name (optional, for debugging/discovery) */
    iox2_node_name_h node_name = NULL;
    if (name && name[0]) {
        iox2_node_name_new(NULL, name, strlen(name), &node_name);
        if (node_name)
            iox2_node_builder_set_name(builder, node_name);
    }

    int ret = iox2_node_builder_create(builder, NULL,
                                       iox2_service_type_e_IPC, &n->handle);
    if (node_name)
        iox2_node_name_drop(node_name);

    if (ret != IOX2_OK) {
        printf("DEL: iox2_node_builder_create failed (err=%d)\n", ret);
        free(n);
        return NULL;
    }

    return n;
}

void del_node_destroy(del_node_t node) {
    if (!node) return;
    if (node->handle)
        iox2_node_drop(node->handle);
    free(node);
}

/* --- Publisher --- */

del_pub_t del_pub_create(del_node_t node, const char *service_name,
                         size_t payload_size) {
    if (!node || !node->handle || !service_name) return NULL;

    del_pub_t p = (del_pub_t)calloc(1, sizeof(struct del_publisher_s));
    if (!p) return NULL;
    p->payload_size = payload_size;

    /* Create service name */
    iox2_service_name_h svc_name = NULL;
    if (iox2_service_name_new(NULL, service_name, strlen(service_name),
                              &svc_name) != IOX2_OK) {
        printf("DEL: Failed to create service name '%s'\n", service_name);
        free(p);
        return NULL;
    }

    /* Build publish-subscribe service */
    iox2_service_builder_h svc_builder =
        iox2_node_service_builder(&node->handle, NULL, svc_name);
    iox2_service_name_drop(svc_name);

    if (!svc_builder) {
        printf("DEL: Failed to create service builder for '%s'\n", service_name);
        free(p);
        return NULL;
    }

    iox2_service_builder_pub_sub_h ps_builder =
        iox2_service_builder_pub_sub(svc_builder);

    iox2_port_factory_pub_sub_h pubsub = NULL;
    int ret = iox2_service_builder_pub_sub_open_or_create(
        ps_builder, NULL, &pubsub);
    if (ret != IOX2_OK || !pubsub) {
        printf("DEL: Failed to open/create pub-sub service '%s' (err=%d)\n",
               service_name, ret);
        free(p);
        return NULL;
    }

    /* Create publisher */
    iox2_port_factory_publisher_builder_h pub_builder =
        iox2_port_factory_pub_sub_publisher_builder(&pubsub, NULL);

    ret = iox2_port_factory_publisher_builder_create(
        pub_builder, NULL, &p->handle);
    if (ret != IOX2_OK || !p->handle) {
        printf("DEL: Failed to create publisher for '%s' (err=%d)\n",
               service_name, ret);
        /* pubsub factory is consumed by the builder */
        free(p);
        return NULL;
    }

    return p;
}

void del_pub_destroy(del_pub_t pub) {
    if (!pub) return;
    if (pub->handle)
        iox2_publisher_drop(pub->handle);
    free(pub);
}

int del_publish(del_pub_t pub, const void *data, size_t size) {
    if (!pub || !pub->handle || !data || size == 0) return -1;

    /* Use send_copy for simplicity - copies data into shared memory */
    size_t recipients = 0;
    int ret = iox2_publisher_send_copy(
        &pub->handle, data, size, &recipients);
    if (ret != IOX2_OK) {
        /* Non-fatal: log but don't stop PLC */
        return -1;
    }
    return 0;
}

/* --- Subscriber --- */

del_sub_t del_sub_create(del_node_t node, const char *service_name,
                         size_t payload_size) {
    if (!node || !node->handle || !service_name) return NULL;

    del_sub_t s = (del_sub_t)calloc(1, sizeof(struct del_subscriber_s));
    if (!s) return NULL;
    s->payload_size = payload_size;

    /* Create service name */
    iox2_service_name_h svc_name = NULL;
    if (iox2_service_name_new(NULL, service_name, strlen(service_name),
                              &svc_name) != IOX2_OK) {
        printf("DEL: Failed to create service name '%s'\n", service_name);
        free(s);
        return NULL;
    }

    /* Build publish-subscribe service */
    iox2_service_builder_h svc_builder =
        iox2_node_service_builder(&node->handle, NULL, svc_name);
    iox2_service_name_drop(svc_name);

    if (!svc_builder) {
        printf("DEL: Failed to create service builder for '%s'\n", service_name);
        free(s);
        return NULL;
    }

    iox2_service_builder_pub_sub_h ps_builder =
        iox2_service_builder_pub_sub(svc_builder);

    iox2_port_factory_pub_sub_h pubsub = NULL;
    int ret = iox2_service_builder_pub_sub_open_or_create(
        ps_builder, NULL, &pubsub);
    if (ret != IOX2_OK || !pubsub) {
        printf("DEL: Failed to open/create pub-sub service '%s' (err=%d)\n",
               service_name, ret);
        free(s);
        return NULL;
    }

    /* Create subscriber */
    iox2_port_factory_subscriber_builder_h sub_builder =
        iox2_port_factory_pub_sub_subscriber_builder(&pubsub, NULL);

    ret = iox2_port_factory_subscriber_builder_create(
        sub_builder, NULL, &s->handle);
    if (ret != IOX2_OK || !s->handle) {
        printf("DEL: Failed to create subscriber for '%s' (err=%d)\n",
               service_name, ret);
        free(s);
        return NULL;
    }

    return s;
}

void del_sub_destroy(del_sub_t sub) {
    if (!sub) return;
    if (sub->handle)
        iox2_subscriber_drop(sub->handle);
    free(sub);
}

int del_receive(del_sub_t sub, void *buf, size_t size) {
    if (!sub || !sub->handle || !buf || size == 0) return -1;

    /* Receive the latest sample */
    iox2_sample_h sample = NULL;
    int ret = iox2_subscriber_receive(&sub->handle, NULL, &sample);
    if (ret != IOX2_OK || !sample) {
        return -1;  /* No data available - not an error */
    }

    /* Copy payload into caller's buffer */
    const void *payload = NULL;
    size_t num_elements = 0;
    iox2_sample_payload(&sample, &payload, &num_elements);

    if (payload) {
        size_t copy_len = (num_elements < size) ? num_elements : size;
        memcpy(buf, payload, copy_len);
    }

    iox2_sample_drop(sample);
    return 0;
}

#else /* !_iceoryx2_src */

/* ================================================================== */
/*  Stub implementation (IceOryx2 not available)                       */
/* ================================================================== */

del_node_t del_node_create(const char *) { return NULL; }
void       del_node_destroy(del_node_t)  { }

del_pub_t  del_pub_create(del_node_t, const char *, size_t) { return NULL; }
void       del_pub_destroy(del_pub_t)    { }
int        del_publish(del_pub_t, const void *, size_t) { return -1; }

del_sub_t  del_sub_create(del_node_t, const char *, size_t) { return NULL; }
void       del_sub_destroy(del_sub_t)    { }
int        del_receive(del_sub_t, void *, size_t) { return -1; }

#endif /* _iceoryx2_src */
