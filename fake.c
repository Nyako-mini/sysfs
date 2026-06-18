// SPDX-License-Identifier: GPL-2.0
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/err.h>

static int fake_enforcing = -1;
static struct kobject *selinux_fake_kobj;

int get_fake_enforcing(void)
{
    return fake_enforcing;
}
EXPORT_SYMBOL(get_fake_enforcing);

static ssize_t fake_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf)
{
    return sprintf(buf, "%d\n", fake_enforcing);
}

static ssize_t fake_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    int val;
    int ret;

    ret = kstrtoint(buf, 10, &val);
    if (ret)
        return ret;

    if (val == 1)
        fake_enforcing = 1;
    else
        fake_enforcing = -1;

    return count;
}

static struct kobj_attribute fake_attr = __ATTR(fake, 0644, fake_show, fake_store);

static int __init fake_init(void)
{
    selinux_fake_kobj = kobject_create_and_add("selinux", kernel_kobj);
    if (!selinux_fake_kobj)
        return -ENOMEM;

    sysfs_create_file(selinux_fake_kobj, &fake_attr.attr);
    return 0;
}
early_initcall(fake_init);
