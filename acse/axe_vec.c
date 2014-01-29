/*
 * Ettore Speziale
 * Politecnico di Milano, 2010
 *             
 * axe_vec.c      
 * Formal Languages & Compilers Machine, 2009/2010
 *             
 */          

#include "axe_vec.h"

void handle_vec_op(t_program_infos *program,
        char *destID, char *sourceID1, char *sourceID2, int vec_op)
{
    t_axe_variable *dest_var = getVariable (program, destID);
    t_axe_variable *source_var1 = getVariable (program, sourceID1);
    t_axe_variable *source_var2 = getVariable (program, sourceID2);

    if (!dest_var->isArray || !source_var1->isArray || !source_var2->isArray)
      notifyError (AXE_INVALID_TYPE);

    if (dest_var->arraySize != source_var1->arraySize ||
        dest_var->arraySize != source_var2->arraySize)
      notifyError (AXE_INVALID_ARRAY_SIZE);

    int index_register, dest_register;
    t_axe_expression index, dest;
    t_axe_label *start, *end;
    
    /* Init loop counter */
    index_register = getNewRegister (program);
    index = create_expression (index_register, REGISTER);
    gen_mova_instruction (program, index_register, NULL,  dest_var->arraySize);

    /* We need a "tmp" register */
    dest_register = getNewRegister (program);
    dest = create_expression (dest_register, REGISTER);

    start = assignNewLabel (program);
    end = newLabel (program);

    /* Whether the arrays have been completely scanned */
    gen_andb_instruction (program, index_register, index_register,
                          index_register, CG_DIRECT_ALL);
    gen_beq_instruction (program, end, 0);

    /* Compute index */
    gen_subi_instruction (program, index_register, index_register, 1);

    /* Load operands */
    int src1_register = loadArrayElement (program, sourceID1, index);
    int src2_register = loadArrayElement (program, sourceID2, index);
    
    /* Perform operation */
    switch (vec_op)
    {
        case VADD:
            gen_add_instruction (program, dest_register, src1_register,
                                 src2_register, CG_DIRECT_ALL);
            break;

        case VSUB:
            gen_sub_instruction (program, dest_register, src1_register,
                                 src2_register, CG_DIRECT_ALL);
            break;

        default:
            notifyError (AXE_INVALID_OPCODE);
    }

    /* Store to destination array */
    storeArrayElement (program, destID, index, dest);

    /* Jump to header */
    gen_bt_instruction (program, start, 0);

    /* Fix exit label */
    assignLabel (program, end);
}
