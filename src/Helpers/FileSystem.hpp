#pragma once
#include "esp_littlefs.h"
#include "esp_log.h"
#include <dirent.h>

class FileSystem
{
private:
    void list_files_in_littlefs() {
        DIR *dir = opendir("/littlefs");
        if (!dir) {
            ESP_LOGE("FS", "Failed to open directory: %s", "/littlefs");
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            ESP_LOGI("FS", "Found file: %s", entry->d_name);
        }
        closedir(dir);
    }
    /* data */
public:
    FileSystem(/* args */);
    ~FileSystem();
    void Init();
};

FileSystem::FileSystem(/* args */)
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::Init()
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .read_only = false,
        .dont_mount = false,
        .grow_on_mount = false,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount or format SPIFFS\n");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            printf("Failed to find SPIFFS partition\n");
        } else {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    } else {
        printf("SPIFFS: Total: %d bytes, Used: %d bytes\n", total, used);
    }

    list_files_in_littlefs();
}