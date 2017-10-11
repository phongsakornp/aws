/*
 * Copyright (c) 2014-2017 Cesanta Software Limited
 * All rights reserved
 */

/*
 * View this file on GitHub:
 * [mgos_aws_shadow.h](https://github.com/mongoose-os-libs/aws/blob/master/src/mgos_aws_shadow.h)
 */

#ifndef CS_MOS_LIBS_AWS_SRC_MGOS_AWS_SHADOW_H_
#define CS_MOS_LIBS_AWS_SRC_MGOS_AWS_SHADOW_H_

#include <stdbool.h>
#include <stdint.h>

#include "common/mg_str.h"
#include "mgos_features.h"
#include "mgos_init.h"

#include "mgos_aws.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mgos_aws_shadow_event {
  MGOS_AWS_SHADOW_CONNECTED = 0,
  MGOS_AWS_SHADOW_GET_ACCEPTED = 1,
  MGOS_AWS_SHADOW_GET_REJECTED = 2,
  MGOS_AWS_SHADOW_UPDATE_ACCEPTED = 3,
  MGOS_AWS_SHADOW_UPDATE_REJECTED = 4,
  MGOS_AWS_SHADOW_UPDATE_DELTA = 5,
};

/*
 * Main AWS Device Shadow state callback handler.
 *
 * Will get invoked when connection is established or when new versions
 * of the state arrive via one of the topics.
 *
 * CONNECTED event comes with no state.
 *
 * For DELTA events, state is passed as "desired", reported is not set.
 */
typedef void (*mgos_aws_shadow_state_handler)(
    void *arg, enum mgos_aws_shadow_event ev, uint64_t version,
    const struct mg_str reported, const struct mg_str desired,
    const struct mg_str reported_md, const struct mg_str desired_md);
typedef void (*mgos_aws_shadow_error_handler)(void *arg,
                                              enum mgos_aws_shadow_event ev,
                                              int code, const char *message);

void mgos_aws_shadow_set_state_handler(mgos_aws_shadow_state_handler state_cb,
                                       void *arg);
void mgos_aws_shadow_set_error_handler(mgos_aws_shadow_error_handler error_cb,
                                       void *arg);

/* Returns ascii name of the event: "CONNECTED", "GET_REJECTED", ... */
const char *mgos_aws_shadow_event_name(enum mgos_aws_shadow_event ev);

typedef bool (*mgos_aws_shadow_state_handler_simple)(
    void *arg, enum mgos_aws_shadow_event ev, const char *reported,
    const char *desired, const char *reported_md, const char *desired_md);

/*
 * "Simple" version of mgos_aws_shadow_set_state_handler, primarily for FFI.
 */
void mgos_aws_shadow_set_state_handler_simple(
    mgos_aws_shadow_state_handler_simple state_cb_simple, void *arg);

/*
 * Request shadow state. Response will arrive via GET_ACCEPTED topic.
 * Note that MGOS automatically does this on every (re)connect if
 * aws.shadow.get_on_connect is true (default).
 */
bool mgos_aws_shadow_get(void);

/*
 * Send an update. Format string should define the value of the "state" key,
 * i.e. it should be an object with "reported" and/or "desired" keys, e.g.:
 * `mgos_aws_shadow_updatef("{reported:{foo: %d, bar: %d}}", foo, bar)`.
 * Response will arrive via UPDATE_ACCEPTED or REJECTED topic.
 * If you want the update to be aplied only if a particular version is current,
 * specify the version. Otherwise set it to 0 to apply to any version.
 */
bool mgos_aws_shadow_updatef(uint64_t version, const char *state_jsonf, ...);

/*
 * "Simple" version of mgos_aws_shadow_updatef, primarily for FFI.
 */
bool mgos_aws_shadow_update_simple(double version, const char *state_json);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CS_MOS_LIBS_AWS_SRC_MGOS_AWS_SHADOW_H_ */
