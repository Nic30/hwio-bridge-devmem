#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <iostream>

#include <hwio/hwio_comp_spec.h>
#include <hwio/hwio_cli.h>
#include <vector>
#include <getopt.h>
#include <assert.h>

using namespace hwio;

#define STAT_FLT_OPTS "d:Alr:w:"

void print_help() {
    std::cout << "hwio_devmem (build " __TIMESTAMP__ ")" << std::endl;
    std::cout << "Read/write from physical address space" << std::endl
    << hwio_help_str() << std::endl << std::endl
    << "Options:" << std::endl
    << "  -d <index>              Use device of selected index" << std::endl
    << "  -A                       Use all compatible devices" << std::endl
    << "  -l                       List all compatible devices" << std::endl
    << "Commands:" << std::endl
    << "Usage: hwio_devmem {compatibility string} ADDRESS [WIDTH [VALUE]]" << std::endl
    << "    (same as devmem + compatibility string)" << std::endl
    << "    {compatibility string}  device-tree compatibility string"    << std::endl
    << "    ADDRESS                    Address to act upon" << std::endl
    << "    WIDTH                    Width (8/16/...), 32 default, WIDTH % 8 == 0, WIDTH > 0"
    << std::endl
    << "    VALUE                    Data to be written" << std::endl
    << "All values can be in dec/hex format, write is limited to 64b due CLI arg. parsing"
    << std::endl
    << "Read data is in hex format" << std::endl;
}

int main(int argc, char **argv) {
    int err = 0;
    auto bus = hwio_init(argc, argv);
    if (bus == nullptr) {
        throw std::runtime_error("Can not initialize HWIO");
    }

    int devIndex = -1;
    bool useAll = false;
    bool listAvailable = false;

    while (true) {
        const auto opt = getopt(argc, argv, STAT_FLT_OPTS);
        if (-1 == opt)
            break;
        switch (opt) {
        case 'h':
            delete bus;
            print_help();
            exit(0);
            break;

        case 'A':
            useAll = true;
            break;

        case 'd':
            devIndex = std::stoi(optarg);
            break;

        case 'l':
            listAvailable = true;
            break;

        default:
            break;
        }
    }

    std::vector<char *> nonOpts;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            nonOpts.push_back(argv[i]);
        }
    }
    if (nonOpts.size() < 1) {
        std::cerr << "[Error] at least compatibility string is required"
                << std::endl;
        print_help();
        delete bus;
        exit(1);
    }

    std::vector<ihwio_dev*> dev_to_use;
    char * comp_str = nonOpts[0];
    std::vector<hwio_comp_spec> compat = { { comp_str } };
    auto devs = bus->find_devices(compat);

    if (listAvailable) {
        std::cout << "Available devices (" << devs.size() << "): " << std::endl;
        for (auto d : devs) {
            std::cout << d->to_str() << std::endl;
            std::cout << "----------------------------" << std::endl;
        }
    }

    if (useAll) {
        for (auto d : devs)
            dev_to_use.push_back(d);
    } else {
    	if (devIndex < 0) {
    		if (devs.size() <= 1) {
    			devIndex = 0;
    		} else {
    			std::cerr << "Platform has multiple devices, device index specification is required (devices_cnt="
    					<< devs.size() << ")" << std::endl;
                delete bus;
                exit(1);
    		}

    	}
        if ((unsigned)devIndex >= devs.size()) {
            std::cerr << "Can not use device " << devIndex
                    << " because platform has only " << devs.size()
                    << " devices" << std::endl;
            delete bus;
            exit(1);
        } else {
            dev_to_use.push_back(devs.at(devIndex));
        }
    }

    assert(dev_to_use.size() > 0);
    if (nonOpts.size() >= 2 && nonOpts.size() <= 4) {
        hwio_phys_addr_t addr = std::stoll(nonOpts.at(1), 0, 0);
        uint64_t data = 0;
        bool write = false;
        size_t size = 4;

        if (nonOpts.size() >= 3) {
            long long _size = std::stoll(nonOpts.at(2), 0, 0);
            if (_size <= 0 || _size % 8 != 0) {
                std::cerr << "[Error] WIDTH is in wrong format got:"
                        << nonOpts.at(2) << std::endl;
                print_help();
                delete bus;
                exit(err);
            }
            size = _size / 8;
        }

        if (nonOpts.size() == 4) {
            write = true;
            data = std::stoll(nonOpts.at(3), 0, 0);
        }

        if (write) {
            assert(size <= 8 && "Not implemented");
            for (auto d : dev_to_use) {
                d->write(addr, &data, size);
            }
        } else {
            uint8_t * buff = new uint8_t[size];
            for (auto d : dev_to_use) {
                std::fill(buff, buff + size, 0);
                d->read(addr, buff, size);
                printf("0x");
                for (int i = size - 1; i >= 0; i--) {
                    printf("%" PRIx8, buff[i]);
                }
                printf("\n");
            }
            delete[] buff;
        }
    } else {
        std::cerr << "[Error] need at least compatibility string and address"
                << std::endl;
        err = 1;
    }

    delete bus;
    return err;
}
