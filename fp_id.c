#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Expose /sys/fp_id/fp_id showing fixed string");

#define FP_ID_VALUE "ultrasonic_fake_nyako"

static struct kobject *fp_id_kobj;

static ssize_t fp_id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return scnprintf(buf, PAGE_SIZE, "%s\n", FP_ID_VALUE);
}

static struct kobj_attribute fp_id_attr = __ATTR_RO(fp_id);

static int __init fp_id_sysfs_init(void)
{
    int ret;

    fp_id_kobj = kobject_create_and_add("fp_id", kernel_kobj);
    if (!fp_id_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(fp_id_kobj, &fp_id_attr.attr);
    if (ret)
        pr_err("fp_id: failed to create fp_id attribute\n");
    else
        pr_info("fp_id: /sys/fp_id/fp_id created with value: %s\n", FP_ID_VALUE);

    return ret;
}

early_initcall(fp_id_sysfs_init);

static void __exit fp_id_sysfs_exit(void)
{
    if (fp_id_kobj) {
        sysfs_remove_file(fp_id_kobj, &fp_id_attr.attr);
        kobject_put(fp_id_kobj);
    }
    pr_info("fp_id: module removed\n");
}

module_exit(fp_id_sysfs_exit);
