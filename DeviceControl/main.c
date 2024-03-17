#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include "scsi.h"
#include "link.h"
#include "thread.h"
#include "msg_handle.h"

thread_handle_t usb_msg_handle_g;
thread_handle_t net_msg_handle_g;

usb_device_handle_t usb_device_g;
int main(int argc, char* argv[])
{
    int res = usb_open(&usb_device_g, "/dev/sdb");
    if(-1 == res)
    {
        printf("usb device open failed\r\n");
        return 0;
    }
    thread_attr_t usb_attr;
    usb_attr.func = (callback)usb_msg_process;
    usb_attr.arg = &usb_msg_handle_g;
    usb_attr.internal_data = &usb_device_g;
    thread_create(&usb_msg_handle_g, &usb_attr);

    thread_attr_t net_attr;
    net_attr.func = net_msg_process;
    net_attr.arg = &net_msg_handle_g;
    thread_create(&net_msg_handle_g, &net_attr);
    while(1)
    {

    }

    return 0;
}

