/*
 * DMA memory management for framework level HCD code (hc_driver)
 *
 * This implementation plugs in through generic "usb_bus" level methods,
 * and works with real PCI, or when "pci device == null" makes sense.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pci.h>


#ifdef CONFIG_USB_DEBUG
	#define DEBUG
#else
	#undef DEBUG
#endif

#include <linux/usb.h>
#include "hcd.h"


/*
 * DMA-Consistent Buffers
 */

/* FIXME tune these based on pool statistics ... */
static const size_t	pool_max [HCD_BUFFER_POOLS] = {
	32,
	128,
	512,
	PAGE_SIZE / 2
	/* bigger --> allocate pages */
};


/* SETUP primitives */

/**
 * hcd_buffer_create - initialize buffer pools
 * @hcd: the bus whose buffer pools are to be initialized
 *
 * Call this as part of initializing a host controller that uses the pci dma
 * memory allocators.  It initializes some pools of dma-consistent memory that
 * will be shared by all drivers using that controller, or returns a negative
 * errno value on error.
 *
 * Call hcd_buffer_destroy() to clean up after using those pools.
 */
int hcd_buffer_create (struct usb_hcd *hcd)
{
	char		name [16];
	int 		i, size;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) { 
		if (!(size = pool_max [i]))
			continue;
		snprintf (name, sizeof name, "buffer-%d", size);
		hcd->pool [i] = pci_pool_create (name, hcd->pdev,
				size, size, 0, SLAB_KERNEL);
		if (!hcd->pool [i]) {
			hcd_buffer_destroy (hcd);
			return -ENOMEM;
		}
	}
	return 0;
}
EXPORT_SYMBOL (hcd_buffer_create);


/**
 * hcd_buffer_destroy - deallocate buffer pools
 * @hcd: the bus whose buffer pools are to be destroyed
 *
 * This frees the buffer pools created by hcd_buffer_create().
 */
void hcd_buffer_destroy (struct usb_hcd *hcd)
{
	int		i;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) { 
		struct pci_pool		*pool = hcd->pool [i];
		if (pool) {
			pci_pool_destroy (pool);
			hcd->pool [i] = 0;
		}
	}
}
EXPORT_SYMBOL (hcd_buffer_destroy);


/* sometimes alloc/free could use kmalloc with SLAB_DMA, for
 * better sharing and to leverage mm/slab.c intelligence.
 */

void *hcd_buffer_alloc (
	struct usb_bus 		*bus,
	size_t			size,
	int			mem_flags,
	dma_addr_t		*dma
)
{
	struct usb_hcd		*hcd = bus->hcpriv;
	int 			i;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		if (size <= pool_max [i])
			return pci_pool_alloc (hcd->pool [i], mem_flags, dma);
	}
	return pci_alloc_consistent (hcd->pdev, size, dma);
}

void hcd_buffer_free (
	struct usb_bus 		*bus,
	size_t			size,
	void 			*addr,
	dma_addr_t		dma
)
{
	struct usb_hcd		*hcd = bus->hcpriv;
	int 			i;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		if (size <= pool_max [i]) {
			pci_pool_free (hcd->pool [i], addr, dma);
			return;
		}
	}
	pci_free_consistent (hcd->pdev, size, addr, dma);
}


/*
 * DMA-Mappings for arbitrary memory buffers
 */

int hcd_buffer_map (
	struct usb_bus	*bus,
	void		*addr,
	dma_addr_t	*dma,
	size_t		size,
	int		direction
) {
	struct usb_hcd	*hcd = bus->hcpriv;

	// FIXME pci_map_single() has no standard failure mode!
	*dma = pci_map_single (hcd->pdev, addr, size,
			(direction == USB_DIR_IN)
				? PCI_DMA_FROMDEVICE
				: PCI_DMA_TODEVICE);
	return 0;
}

void hcd_buffer_dmasync (
	struct usb_bus	*bus,
	dma_addr_t	dma,
	size_t		size,
	int		direction
) {
	struct usb_hcd *hcd = bus->hcpriv;

	pci_dma_sync_single (hcd->pdev, dma, size,
			(direction == USB_DIR_IN)
				? PCI_DMA_FROMDEVICE
				: PCI_DMA_TODEVICE);
}

void hcd_buffer_unmap (
	struct usb_bus	*bus,
	dma_addr_t	dma,
	size_t		size,
	int		direction
) {
	struct usb_hcd *hcd = bus->hcpriv;

	pci_unmap_single (hcd->pdev, dma, size,
			(direction == USB_DIR_IN)
				? PCI_DMA_FROMDEVICE
				: PCI_DMA_TODEVICE);
}

int hcd_buffer_map_sg (
	struct usb_bus		*bus,
	struct scatterlist	*sg,
	int			*n_hw_ents,
	int			nents,
	int			direction
) {
	struct usb_hcd *hcd = bus->hcpriv;

	// FIXME pci_map_sg() has no standard failure mode!
	*n_hw_ents = pci_map_sg(hcd->pdev, sg, nents,
				(direction == USB_DIR_IN)
				? PCI_DMA_FROMDEVICE
				: PCI_DMA_TODEVICE);
	return 0;
}

void hcd_buffer_sync_sg (
	struct usb_bus		*bus,
	struct scatterlist	*sg,
	int			n_hw_ents,
	int			direction
) {
	struct usb_hcd *hcd = bus->hcpriv;

	pci_dma_sync_sg(hcd->pdev, sg, n_hw_ents,
			(direction == USB_DIR_IN)
			? PCI_DMA_FROMDEVICE
			: PCI_DMA_TODEVICE);
}

void hcd_buffer_unmap_sg (
	struct usb_bus		*bus,
	struct scatterlist	*sg,
	int			n_hw_ents,
	int			direction
) {
	struct usb_hcd *hcd = bus->hcpriv;

	pci_unmap_sg(hcd->pdev, sg, n_hw_ents,
		     (direction == USB_DIR_IN)
		     ? PCI_DMA_FROMDEVICE
		     : PCI_DMA_TODEVICE);
}