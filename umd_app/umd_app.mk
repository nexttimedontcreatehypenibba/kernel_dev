UMD_APP_VERSION = 1.0
UMD_APP_SITE = ./package/umd_app/src
UMD_APP_SITE_METHOD = local

define UMD_APP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define UMD_APP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/umd_app $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
