#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>

static int __init init(void);
static void __exit cleanup(void);

static int device_open(struct inode *inode, struct file *file) {
  pr_info("pz3: Device opened\n");
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
  pr_info("pz3: Device released\n");
  return 0;
}

const struct file_operations fops = {
    .open = device_open,
    .release = device_release,
};

unsigned int Major;
static struct class *dev_class;
static struct device *dev_device;

static int __init init(void) {
  pr_info("pz3: Initializing driver\n");

  Major = register_chrdev(0, "pz3", &fops);
  if (Major < 0) {
    pr_alert("pz3: Registering char device failed with %d\n", Major);
    return Major;
  }
  
  pr_info("pz3: Registered with major number %d\n", Major);
  
  // ДЛЯ ЯДРА 6.x: class_create принимает ТОЛЬКО имя класса!
  dev_class = class_create("pz3_class");
  if (IS_ERR(dev_class)) {
    pr_alert("pz3: Failed to create device class\n");
    unregister_chrdev(Major, "pz3");
    return PTR_ERR(dev_class);
  }
  
  dev_device = device_create(dev_class, NULL, MKDEV(Major, 0), NULL, "pz3");
  if (IS_ERR(dev_device)) {
    pr_alert("pz3: Failed to create device\n");
    class_destroy(dev_class);
    unregister_chrdev(Major, "pz3");
    return PTR_ERR(dev_device);
  }
  
  pr_info("pz3: Driver initialized successfully\n");
  pr_info("pz3: Device created at /dev/pz3\n");
  
  return 0;
}

static void __exit cleanup(void) {
  pr_info("pz3: Cleaning up driver\n");
  
  if (dev_device)
    device_destroy(dev_class, MKDEV(Major, 0));
  
  if (dev_class)
    class_destroy(dev_class);
  
  if (Major > 0)
    unregister_chrdev(Major, "pz3");
    
  pr_info("pz3: Driver unloaded\n");
}

module_init(init);
module_exit(cleanup);

MODULE_DESCRIPTION("MAI drivers training");
MODULE_LICENSE("GPL");
