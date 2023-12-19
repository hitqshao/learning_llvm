create the control flow graph of IR

0) initialize
   create a map mapping from BasicBlock* to ID

1) iterate through Function
    1.1) iterate through blocks
        check if block is in basicBlockMap
        if in:
            get Id
        else
            create ID++

        utilize successors(Blk*) to iterate successor
        this is to collect edges

        if sucessor block is not iterated yet
            create id for it
        else
            use its id

            connect the block with successor block
