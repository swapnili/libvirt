 /*
 * virfirewall.h: integration with firewalls
 *
 * Copyright (C) 2014 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "internal.h"

typedef struct _virFirewall virFirewall;

typedef struct _virFirewallCmd virFirewallCmd;

typedef enum {
    VIR_FIREWALL_LAYER_ETHERNET,
    VIR_FIREWALL_LAYER_IPV4,
    VIR_FIREWALL_LAYER_IPV6,

    VIR_FIREWALL_LAYER_LAST,
} virFirewallLayer;

virFirewall *virFirewallNew(void);

void virFirewallFree(virFirewall *firewall);

/**
 * virFirewallAddCmd:
 * @firewall: firewall ruleset to add to
 * @layer: the firewall layer to change
 * @...: NULL terminated list of strings for the rule
 *
 * Add any type of rule to the firewall ruleset.
 *
 * Returns the new rule
 */
#define virFirewallAddCmd(firewall, layer, ...) \
         virFirewallAddCmdFull(firewall, layer, false, NULL, NULL, __VA_ARGS__)

typedef int (*virFirewallQueryCallback)(virFirewall *firewall,
                                        virFirewallLayer layer,
                                        const char *const *lines,
                                        void *opaque);

virFirewallCmd *virFirewallAddCmdFull(virFirewall *firewall,
                                      virFirewallLayer layer,
                                      bool ignoreErrors,
                                      virFirewallQueryCallback cb,
                                      void *opaque,
                                      ...)
    G_GNUC_NULL_TERMINATED;

void virFirewallRemoveCmd(virFirewall *firewall,
                          virFirewallCmd *rule);

void virFirewallCmdAddArg(virFirewall *firewall,
                          virFirewallCmd *rule,
                          const char *arg)
    ATTRIBUTE_NONNULL(3);

void virFirewallCmdAddArgFormat(virFirewall *firewall,
                                virFirewallCmd *rule,
                                const char *fmt, ...)
    ATTRIBUTE_NONNULL(3) G_GNUC_PRINTF(3, 4);

void virFirewallCmdAddArgSet(virFirewall *firewall,
                             virFirewallCmd *rule,
                             const char *const *args)
    ATTRIBUTE_NONNULL(3);

void virFirewallCmdAddArgList(virFirewall *firewall,
                              virFirewallCmd *rule,
                              ...)
    G_GNUC_NULL_TERMINATED;

size_t virFirewallCmdGetArgCount(virFirewallCmd *rule);

char *virFirewallCmdToString(const char *cmd,
                             virFirewallCmd *rule);

typedef enum {
    /* Ignore all errors when applying rules, so no
     * rollback block will be required */
    VIR_FIREWALL_TRANSACTION_IGNORE_ERRORS = (1 << 0),
} virFirewallTransactionFlags;

void virFirewallStartTransaction(virFirewall *firewall,
                                 unsigned int flags);

typedef enum {
    /* Execute previous rollback block before this
     * one, to chain cleanup */
    VIR_FIREWALL_ROLLBACK_INHERIT_PREVIOUS = (1 << 0),
} virFirewallRollbackFlags;

void virFirewallStartRollback(virFirewall *firewall,
                              unsigned int flags);

int virFirewallApply(virFirewall *firewall);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(virFirewall, virFirewallFree);
