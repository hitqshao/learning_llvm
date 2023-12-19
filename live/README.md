1) initialization

    //map values to their bitvector. Each values has it's own bit vector
    //This only helps when you want to printout, by indexing this vector
    // you will get the argument
    bvIndexToInstrArg = new std::vector<Value*>();

    //map values (args and variables) to their bit vector index
    valueToBitVectorIndex = new ValueMap<Value*, int>();

    // The above two variable are just dictionary, mapping from instruction/arg to index
    // This below one is the core part, it record the live of all args in each instruction

    //In set for a instruction inside basic block : lower level of granularity
    ValueMap<const Instruction*, BitVector*> *instrInSet;

    1.1) iterate through all args in fuction
        bvIndexToInstrArg->push_back(&*arg);
        (*valueToBitVectorIndex)[&*arg] = index;

    1.2) iterate through instructions in function
        bvIndexToInstrArg->push_back(&*instruction);
        (*valueToBitVectorIndex)[&*instruction] = index;

        (*instrInSet)[&*instruction] = new BitVector(domainSize, false);


valueToBitVectorIndex maps arg to index, this will be used in transferFn

2) transferFn
    (*immIn)[(*valueToBitVectorIndex)[*User::op_iterator]] = true;

    1) (*valueToBitVectorIndex)[*User::op_iterator] will get the index for argument in this instruction
    2) (*immIn)[index] = true;
        currently, immIn points to -> immIn = (*instrInSet)[tempInst];
        by setting this as true, it update the index element in BitVector for current instruction
        (ValueMap<const Instruction*, BitVector*>) as true.
        this means that for every instruction, they have a BitVector that records the live/dead of all values
        in the whole program.

        By processing backwords, it follows this rule:
            IN[n] = USE[n] U (OUT[n] - DEF[n])

        and after processed current instruction, it will be OUT, and copy its BitVector to next instruction,
        the next instruction will update its BitVector according to it definition and use:
            set use as true
            set definition as false
