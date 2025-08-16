# dmatest

Just some code to buid a kernel module to play around with the DMA bus. The actual intent is to create an RDMA demo eventually.

1. **Direct Piping Interface**:
   - Write to `/dev/dma_in` to send data
   - Read from `/dev/dma_out` to receive data

2. **DMA Buffer Management**:
   - Automatically allocates DMA buffers
   - Handles buffer size limitations

3. **Progress Reporting**:
   - Kernel logs show transfer progress
   - Percentage and byte counts reported

4. **IOCTL Commands**:
   - `DMA_GET_PHYS_ADDR`: Get physical DMA address
   - `DMA_GET_BUFFER_SIZE`: Get buffer size
   - `DMA_TRANSFER_COMPLETE`: Signal transfer completion

5. **Synchronization**:
   - Proper mutex protection for shared data
   - Wait queues for blocking operations

Usage example:
```bash
# Load the module
sudo insmod dma_pipe.ko

# Send data to DMA input
cat input_file > /dev/dma_in

# Receive data from DMA output
cat /dev/dma_out > output_file

# Monitor progress
dmesg | tail

3. **Verification Steps**:
```bash
# Check kernel version
uname -r

# Clean rebuild
make clean && make

# Load module
sudo insmod dma_transfer.ko

# Verify
dmesg | tail
ls -l /dev/dma_transfer
```

This solution:
- Maintains all DMA functionality
- Works across different kernel versions
- Includes proper error handling and cleanup
- Provides clear debug output

The module creates `/dev/dma_transfer` which can be used with standard file operations while utilizing DMA buffers for data transfer.
** NOTE: You'll need a special NIC (InfiniBand or a RoCEv2-capable card to implement RDMA between hosts). 
