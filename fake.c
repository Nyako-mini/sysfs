// SPDX-License-Identifier: GPL-2.0
/*
 * Kernel Fake Enforcing State
 *
 * This provides a sysfs interface (/sys/kernel/fake) for user space
 * to read or set a "fake" enforcing state. This does not change the
 * real kernel state.
 *
 * The value semantics:
 *   - -1: Unset, return the real state (read-only)
 *   -  0: Force return 0
 *   -  1: Force return 1
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/err.h>

static int fake_enforcing = -1;
static struct kobject *fake_kobj;

/* Determine the value to display via the sysfs interface. */
int get_fake_enforcing(void)
{
	if (fake_enforcing >= 0)
		return fake_enforcing;
	return -1;  /* Indicates using real state */
}
EXPORT_SYMBOL(get_fake_enforcing);

/* Show the current fake state. */
static ssize_t fake_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf)
{
	return sprintf(buf, "%d\n", get_fake_enforcing());
}

/* Store a new fake state. */
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
	} else if (val == 0) {
		fake_enforcing = -1; /* Reset to use the real state */
	} else {
		return -EINVAL;
	}

	return count;
}

static struct kobj_attribute fake_attr = __ATTR(fake, 0644, fake_show, fake_store);

/* Create the fake sysfs interface. */
static int __init fake_init(void)
{
	int ret;

	fake_kobj = kobject_create_and_add("fake", kernel_kobj);
	if (!fake_kobj) {
		pr_err("Fake: failed to create fake kobject\n");
		return -ENOMEM;
	}

	ret = sysfs_create_file(fake_kobj, &fake_attr.attr);
	if (ret) {
		pr_err("Fake: failed to create fake sysfs file\n");
		kobject_put(fake_kobj);
		return ret;
	}

	pr_info("Fake: /sys/kernel/fake created\n");
	return 0;
}
early_initcall(fake_init);
