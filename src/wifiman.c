#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <regex.h>

#define WIFI_INTERFACE_PATTERN "^wlp[0-9]+s[0-9]+|^wlan[0-9]+"

int wifi_is_enabled() {
    struct ifaddrs *ifaddr, *ifa;
    regex_t regex;
    int result;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    if (regcomp(&regex, WIFI_INTERFACE_PATTERN, REG_EXTENDED) != 0) {
        printf("Failed to compile regex.\n");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name != NULL && regexec(&regex, ifa->ifa_name, 0, NULL, 0) == 0) {
            freeifaddrs(ifaddr);
            regfree(&regex);
            return 1;
        }
    }

    freeifaddrs(ifaddr);
    regfree(&regex);

    return 0;
}

void write_to_file(const char* filename, const char* str) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fprintf(file, "%s", str);
    fclose(file);
}

void enable_wifi() {
    write_to_file("/sys/module/wlan/parameters/fwpath", "");
    usleep(100000);
    write_to_file("/sys/module/wlan/parameters/fwpath", "sta");
}

int main() {
    while (1) {
        if (!wifi_is_enabled()) {
            printf("dead, re-enabling");
            enable_wifi();
        }
        sleep(1);
    }
    return 0;
}
