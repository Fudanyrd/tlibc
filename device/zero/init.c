#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
   
int register_device(void);
void unregister_device(void);

static const char buf[512] = { 0 };

/** Register the device, and set buffer to zero. */
static int my_init(void)
{
    register_device();
    return 0;
}
   
static void my_exit(void)
{
    return;
}
   
module_init(my_init);
module_exit(my_exit);

static ssize_t device_file_read(
                        struct file *file_ptr
                       , char __user *user_buffer
                       , size_t count
                       , loff_t *position) {
    size_t bread;
    // total number of bytes read
    size_t tread = 0;
    while (count != 0) {
        bread = sizeof(buf) > count ? count : sizeof(buf);
        if( copy_to_user(user_buffer, buf, bread) != 0 )
            return -EFAULT;    
        count -= bread;
        tread += bread;
        user_buffer += bread;
    }
    /* Move reading position */
    *position += tread;
    return tread;
}

static ssize_t device_file_write(struct file *file_ptr, 
                                 const char __user *user_buffer, 
                                 size_t count, loff_t *position) {
    return count;
}

static struct file_operations zero_fops = {
    .owner   = THIS_MODULE,
    .read    = device_file_read,
    .write   = device_file_write,
};

static int device_file_major_number = 0;
static const char device_name[] = "zero";

int register_device(void) {
    int result = 0;
    result = register_chrdev( 0, device_name, &zero_fops );
    if (result < 0) {
        printk( KERN_WARNING "zero:  can't register character device with error code = %in", result );
        return result;
    }
    device_file_major_number = result;
    return 0;
}

void unregister_device(void) {
    printk ( KERN_NOTICE "zero: unregister_device() is calledn" );
    if(device_file_major_number != 0) {
        unregister_chrdev(device_file_major_number, device_name);
    }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yrd");
