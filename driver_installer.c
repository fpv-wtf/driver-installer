#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>  

#include <conio.h>

#include "libwdi.h"

struct target
{
    unsigned short vid;
    unsigned short pid;
    bool is_composite;
    unsigned char iface;
    char* vendor_name;
    char* description; 
    char* inf_name;
    int driver_type;

};

int numtargets=3;
static struct target targets[3] = {
                    {0x2CA3, 0x40, false, 0, "DJI", "DJI Recovery", "dji_recovery.inf", WDI_LIBUSB0},
                    {0x18D1, 0xD00D, false, 0, "DJI", "Kedacom Fastboot/ADB", "android_winusb.inf", WDI_USER},
                    {0x2CA3, 0x001F, true, 0x03, "DJI", "DJI RMVT Bulk", "dji_rmvt_bulk.inf", WDI_WINUSB}
                };

void waitforexit(int r) {
    printf("Press any key to close the installer.\n");
    getch(); 
    exit(r);
}

void diagnostics() {
    printf("Scanning for DJI Recovery interface ignoring drivers. Please plug in your device now.\n");
    static struct wdi_options_create_list ocl = { 0 };
    ocl.list_all = TRUE;
	ocl.list_hubs = TRUE;
	ocl.trim_whitespaces = TRUE;
    static struct wdi_device_info *ldev = {NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL};
    int r = 0;

    while (1) {
        if (wdi_create_list(&ldev, &ocl) == WDI_SUCCESS) {
            r = WDI_SUCCESS;
            for (; (ldev != NULL) && (r == WDI_SUCCESS); ldev = ldev->next) {
                if ( (ldev->vid == targets[0].vid) && (ldev->pid ==  targets[0].pid) ) {
                    
                    printf("Found DJI Recovery device:\n");

                    printf("vid: 0x%08x\n", ldev->vid);
                    printf("pid: 0x%08x\n", ldev->pid);
                    printf("is_composite: %d\n", ldev->is_composite);
                    printf("mi: %d\n", ldev->mi);
                    printf("description: %s\n", ldev->desc);
                    printf("driver: %s\n", ldev->driver);
                    printf("device_id: %s\n", ldev->device_id);
                    printf("hardware_id: %s\n", ldev->hardware_id);
                    printf("compatible_id : %s\n", ldev->compatible_id);
                    printf("upper_filter : %s\n", ldev->upper_filter);
                    printf("driver_version : %lld\n", ldev->driver_version);

                    fflush(stdout);
                    waitforexit(0);
                }
            }
        }        
        sleep(0.3);
}
}

int main(int argc, char **argv) {
    //increase libwdi log level
    if(argc == 2 && strncmp(argv[1], "-v", 2) == 0) {
        wdi_set_log_level(WDI_LOG_LEVEL_DEBUG);
    }
    else {
        wdi_set_log_level(WDI_LOG_LEVEL_NONE);
    }

    //do a diagnostics run
    if(argc == 2 && strncmp(argv[1], "-d", 2) == 0) {
        diagnostics();
    }
    
    struct target* ptr = targets;
    for (int i=0; i<numtargets; i++, ptr++ ) {
        struct wdi_device_info recovery_dev = {NULL, ptr->vid, ptr->pid, ptr->is_composite, ptr->iface, ptr->description, NULL, NULL, NULL, NULL, NULL, 0};
        struct wdi_device_info *ldev = {NULL, ptr->vid, ptr->pid, FALSE, 0, ptr->description, NULL, NULL, NULL};
        int r;
        static struct wdi_options_create_list ocl = { 0 };
        static struct wdi_options_prepare_driver opd = { 0 };
        static struct wdi_options_install_driver oid = { 0 };
        opd.driver_type = ptr->driver_type;
        opd.vendor_name = ptr->vendor_name;

        static BOOL matching_device_found;

        r = wdi_prepare_driver(&recovery_dev, NULL, ptr->inf_name, &opd);
        if (r != WDI_SUCCESS) {
            printf("Error: Unable to prepare %s driver for installation: %s.\n", ptr->description, wdi_strerror(r));
            waitforexit(1);
        }
        printf("Installing %s driver, this may take a few minutes.\n", ptr->description);

        
        // Try to match against a plugged device to avoid device manager prompts
        matching_device_found = FALSE;
        if (wdi_create_list(&ldev, &ocl) == WDI_SUCCESS) {
            r = WDI_SUCCESS;
            for (; (ldev != NULL) && (r == WDI_SUCCESS); ldev = ldev->next) {
                if ( (ldev->vid == recovery_dev.vid) && (ldev->pid == recovery_dev.pid) && (ldev->mi == recovery_dev.mi) &&(ldev->is_composite == recovery_dev.is_composite) ) {
                    
                    recovery_dev.hardware_id = ldev->hardware_id;
                    recovery_dev.device_id = ldev->device_id;
                    matching_device_found = TRUE;
                    printf("Found %s device already plugged in, replacing driver.\n", ptr->description);
                    r = wdi_install_driver(&recovery_dev, NULL, ptr->inf_name, &oid);
                    if(r != WDI_SUCCESS) {
                        printf("Error: Unable to install %s driver: %s.\n", ptr->description, wdi_strerror(r));
                        waitforexit(1);
                    }
                }
            }
        }

        // No plugged USB device matches this one -> install driver
        if (!matching_device_found) {
            r = wdi_install_driver(&recovery_dev, NULL, ptr->inf_name, &oid);
            if(r != WDI_SUCCESS) {
                printf("Error: Unable to install %s driver: %s.\n", ptr->description, wdi_strerror(r));
                waitforexit(1);
            }
        }

       
        printf("%s driver installed successfully.\n\n", ptr->description);

    }

    waitforexit(0);
 
}
