require u-boot-mender-common.inc

FILES_${PN}_append_mender-uboot = " /data/u-boot/fw_env.config"

do_compile_append_mender-uboot() {
    alignment_bytes=${MENDER_PARTITION_ALIGNMENT}
    if [ $(expr ${MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET} % $alignment_bytes) -ne 0 ]; then
        bberror "MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET must be aligned to" \
                "MENDER_PARTITION_ALIGNMENT"
    fi

    if [ ! -e ${WORKDIR}/fw_env.config.default ]; then
        mender_create_fw_env_config_file ${WORKDIR}/fw_env.config
    else
        cp ${WORKDIR}/fw_env.config.default ${WORKDIR}/fw_env.config
    fi
}

do_install_append_mender-uboot() {
    install -d -m 755 ${D}${sysconfdir}
    ln -sf /data/u-boot/fw_env.config ${D}${sysconfdir}/fw_env.config

    install -d ${D}/data/u-boot
    install -m 0644 ${WORKDIR}/fw_env.config ${D}/data/u-boot/fw_env.config
    if [ ${MENDER_UBOOT_CONFIG_SYS_MMC_ENV_PART} != 0 ]; then
        install -d ${D}${sysconfdir}/profile.d/
        echo 'fw_setenv() {' > ${WORKDIR}/fw_unlock_mmc.sh
        echo "      echo 0 > /sys/block/${MENDER_UBOOT_MMC_ENV_LINUX_DEVICE}/force_ro" >> ${WORKDIR}/fw_unlock_mmc.sh
        echo '      /sbin/fw_setenv "$@"' >> ${WORKDIR}/fw_unlock_mmc.sh
        echo "      echo 1 > /sys/block/${MENDER_UBOOT_MMC_ENV_LINUX_DEVICE}/force_ro" >> ${WORKDIR}/fw_unlock_mmc.sh
        echo '}' >> ${WORKDIR}/fw_unlock_mmc.sh
        install -m 0644 ${WORKDIR}/fw_unlock_mmc.sh ${D}${sysconfdir}/profile.d/fw_unlock_mmc.sh
    fi
}
