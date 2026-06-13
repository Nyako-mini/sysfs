#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/uaccess.h>

#define SRC_SOH "/sys/devices/virtual/oplus_chg/battery/battery_soh"
#define SRC_CYCLE "/sys/devices/virtual/oplus_chg/battery/battery_cc"

static struct class *fuel_class;

static ssize_t read_source_file(const char *path, char *buf, size_t count)
{
    struct file *file;
    ssize_t ret;
    loff_t pos = 0;
    char *tmp_buf;

    file = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(file))
        return PTR_ERR(file);

    tmp_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!tmp_buf) {
        filp_close(file, NULL);
        return -ENOMEM;
    }

    ret = kernel_read(file, tmp_buf, PAGE_SIZE - 1, &pos);
    
    if (ret >= 0) {
        tmp_buf[ret] = '\0';
        snprintf(buf, count, "%s", tmp_buf);
    }

    kfree(tmp_buf);
    filp_close(file, NULL);
    return ret;
}

static ssize_t soh_show(const struct class *cls, 
                        const struct class_attribute *attr, 
                        char *buf)
{
    return read_source_file(SRC_SOH, buf, PAGE_SIZE);
}

static ssize_t cycle_show(const struct class *cls, 
                          const struct class_attribute *attr, 
                          char *buf)
{
    return read_source_file(SRC_CYCLE, buf, PAGE_SIZE);
}

static CLASS_ATTR_RO(soh);
static CLASS_ATTR_RO(cycle);

static int __init fuelsummary_init(void)
{
    int ret;

    fuel_class = class_create("fuelsummary");
    if (IS_ERR(fuel_class))
        return PTR_ERR(fuel_class);

    ret = class_create_file(fuel_class, &class_attr_soh);
    if (ret)
        goto err_soh;

    ret = class_create_file(fuel_class, &class_attr_cycle);
    if (ret)
        goto err_cycle;

    return 0;

err_cycle:
    class_remove_file(fuel_class, &class_attr_soh);
err_soh:
    class_destroy(fuel_class);
    return ret;
}

static void __exit fuelsummary_exit(void)
{
    class_remove_file(fuel_class, &class_attr_cycle);
    class_remove_file(fuel_class, &class_attr_soh);
    class_destroy(fuel_class);
}

module_init(fuelsummary_init);
module_exit(fuelsummary_exit);

MODULE_LICENSE("GPL");
