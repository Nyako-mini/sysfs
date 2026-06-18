// SPDX-License-Identifier: GPL-2.0
/*
 * Kernel Fake Enforcing State
 *
 * This provides a sysfs interface (/sys/selinux/fake) for user space
 * to read or set a "fake" enforcing state. This does not change the
 * real kernel enforcing mode.
 *
 * The value semantics:
 *   - 0: Use real enforcing state (default)
 *   - 1: Force return 1 (enforcing) for fake detection
 *
 * Note: This only affects the value returned by get_fake_enforcing(),
 *       not the actual SELinux enforcing state.
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/slab.h>

static int fake_enforcing = 0;
static struct kobject *selinux_fake_kobj;

int get_fake_enforcing(void)
{
	if (fake_enforcing == 1)
		return 1;
	return -1;
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

	if (val == 1) {
		fake_enforcing = 1;
		pr_info("Fake: enforcing detection set to 1 (fake)\n");
	} else if (val == 0) {
		fake_enforcing = 0;
		pr_info("Fake: enforcing detection set to 0 (real)\n");
	} else {
		return -EINVAL;
	}

	return count;
}

static struct kobj_attribute fake_attr = __ATTR(fake, 0644, fake_show, fake_store);

static int __init fake_init(void)
{
	int ret;

	selinux_fake_kobj = kobject_create_and_add("selinux", kernel_kobj);
	if (!selinux_fake_kobj) {
		pr_err("Fake: failed to create selinux kobject\n");
		return -ENOMEM;
	}

	ret = sysfs_create_file(selinux_fake_kobj, &fake_attr.attr);
	if (ret) {
		pr_err("Fake: failed to create fake sysfs file\n");
		kobject_put(selinux_fake_kobj);
		return ret;
	}

	pr_info("Fake: /sys/selinux/fake created (0=real, 1=fake)\n");
	return 0;
}
early_initcall(fake_init);
