#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static int device_file_major_number = 0;
static const char device_name[] = "null";

static ssize_t null_read(
                        struct file *file_ptr, 
                        char __user *user_buffer, 
                        size_t count, 
                        loff_t *position) { return 0; } 

static ssize_t null_write(
                        struct file *file_ptr, 
                        const char __user *user_buffer, 
                        size_t count, 
                        loff_t *position) { 
    *position += count;
    return count; 
} 

static struct file_operations null_ops = {
    .owner   = THIS_MODULE,
    .read    = null_read,
    .write   = null_write,
};

static int my_init(void) {
    int result = 0;
    result = register_chrdev( 0, device_name, &null_ops );
    if (result < 0) {
        printk( KERN_WARNING "zero:  can't register character device with error code = %in", result );
        return result;
    }
    device_file_major_number = result;
    return 0;
}

static void my_exit(void) {};

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("yrd");
