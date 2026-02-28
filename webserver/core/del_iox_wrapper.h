/*
 * del_iox_wrapper.h - DEL thin wrapper around IceOryx2 C binding
 *
 * Part of the Data Exchange Layer (DEL) for OpenPLC v3.
 *
 * This header defines a minimal, stable C API that the auto-generated
 * iceoryx2_bridge.cpp uses. The actual IceOryx2 C FFI details are
 * encapsulated in del_iox_wrapper.cpp, isolating the generated code
 * from IceOryx2 API changes.
 *
 * Copyright (C) 2025 OpenPLC DEL Contributors
 * Licensed under GPL v3 (same as OpenPLC v3)
 */

#ifndef DEL_IOX_WRAPPER_H
#define DEL_IOX_WRAPPER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle types */
typedef struct del_node_s*       del_node_t;
typedef struct del_publisher_s*  del_pub_t;
typedef struct del_subscriber_s* del_sub_t;

/*
 * Create an IceOryx2 node with the given name.
 * Returns NULL on failure.
 */
del_node_t del_node_create(const char *name);

/*
 * Destroy the IceOryx2 node and release all resources.
 */
void del_node_destroy(del_node_t node);

/*
 * Create a publisher on the given service name with the specified
 * maximum payload size. Returns NULL on failure.
 */
del_pub_t del_pub_create(del_node_t node, const char *service_name,
                         size_t payload_size);

/*
 * Destroy a publisher.
 */
void del_pub_destroy(del_pub_t pub);

/*
 * Publish data by copying `size` bytes from `data` to a loaned sample
 * and sending it. Returns 0 on success, -1 on failure.
 */
int del_publish(del_pub_t pub, const void *data, size_t size);

/*
 * Create a subscriber on the given service name with the specified
 * maximum payload size. Returns NULL on failure.
 */
del_sub_t del_sub_create(del_node_t node, const char *service_name,
                         size_t payload_size);

/*
 * Destroy a subscriber.
 */
void del_sub_destroy(del_sub_t sub);

/*
 * Receive the latest sample and copy up to `size` bytes into `buf`.
 * Returns 0 if a sample was received, -1 if no data available.
 */
int del_receive(del_sub_t sub, void *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* DEL_IOX_WRAPPER_H */
