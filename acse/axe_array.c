/*
 * Andrea Di Biagio
 * Politecnico di Milano, 2007
 * 
 * axe_array.c
 * Formal Languages & Compilers Machine, 2007/2008
 * 
 */

#include "axe_array.h"
#include "axe_gencode.h"
#include "symbol_table.h"
#include "axe_utils.h"
#include "axe_errors.h"
#include "axe_expressions.h"

void storeArrayElement(t_program_infos *program, char *ID
            , t_axe_expression index, t_axe_expression data)
{
   int address;
   
   address =  loadArrayAddress(program, ID, index);

   if (data.expression_type == REGISTER)
   {
      /* load the value indirectly into `mova_register' */
      gen_add_instruction(program, address, REG_0
               , data.value, CG_INDIRECT_DEST);
   }
   else
   {
      int imm_register;

      imm_register = gen_load_immediate(program, data.value);

      /* load the value indirectly into `load_register' */
      gen_add_instruction(program, address, REG_0
               ,imm_register, CG_INDIRECT_DEST);
   }
}

int loadArrayElement(t_program_infos *program
               , char *ID, t_axe_expression index)
{
   int load_register;
   int address;

   /* retrieve the address of the array slot */
   address = loadArrayAddress(program, ID, index);

   /* get a new register */
   load_register = getNewRegister(program);

   /* load the value into `load_register' */
   gen_add_instruction(program, load_register, REG_0
            , address, CG_INDIRECT_SOURCE);

   /* return the register ID that holds the required data */
   return load_register;
}

int loadArrayAddress(t_program_infos *program
            , char *ID, t_axe_expression index)
{
   int mova_register;
   t_axe_label *label;

   /* preconditions */
   if (program == NULL)
      notifyError(AXE_PROGRAM_NOT_INITIALIZED);

   if (ID == NULL)
      notifyError(AXE_VARIABLE_ID_UNSPECIFIED);
   
   /* retrieve the label associated with the given
   * identifier */
   label = getLabelFromVariableID(program, ID);
                     
   /* test if an error occurred */
   if (label == NULL)
      return REG_INVALID;

   /* get a new register */
   mova_register = getNewRegister(program);

   /* generate the MOVA instruction */
   gen_mova_instruction(program, mova_register, label, 0);

   if (index.expression_type == IMMEDIATE)
   {
      if (index.value != 0)
      {
         gen_addi_instruction (program, mova_register
                  , mova_register, index.value);
      }
   }
   else
   {
      assert(index.expression_type == REGISTER);

      /* We are making the following assumption:
      * the type can only be an INTEGER_TYPE */
      gen_add_instruction(program, mova_register, mova_register
               , index.value, CG_DIRECT_ALL);
   }

   /* return the identifier of the register that contains
    * the value of the array slot */
   return mova_register;
}


extern void mirrorArray(t_program_infos *program, t_axe_variable* array, 
                        t_axe_expression start, t_axe_expression end) {
    
    int from_reg, to_reg, result, el1_reg, el2_reg;
    t_axe_expression from_expr, to_expr, el1_expr, el2_expr;
    t_axe_label *condition_label, *end_label;
    
    /* Compute initial indexes */
    if(start.expression_type == IMMEDIATE) {
        from_reg = gen_load_immediate(program, start.value);
    }
    else {
        from_reg = getNewRegister(program);
        gen_andb_instruction(program, from_reg, start.value, start.value, CG_DIRECT_ALL);
    }
    from_expr = create_expression(from_reg, REGISTER);
    
    if(end.expression_type == IMMEDIATE) {
        to_reg = gen_load_immediate(program, end.value);
    }
    else {
        to_reg = getNewRegister(program);
        gen_andb_instruction(program, to_reg, end.value, end.value, CG_DIRECT_ALL);
    }
    to_expr = create_expression(to_reg, REGISTER);
    gen_subi_instruction(program, to_reg, to_reg, 1);
    
    /* Check condition */
    condition_label = assignNewLabel(program);
    
    result = getNewRegister(program);
    gen_sub_instruction(program, result, to_reg, from_reg, CG_DIRECT_ALL);
        
    end_label = newLabel(program);
    gen_ble_instruction(program, end_label, 0);
    
    /* Swap */
    el1_reg = loadArrayElement(program, array->ID, from_expr);
    el1_expr = create_expression(el1_reg, REGISTER);
    
    el2_reg = loadArrayElement(program, array->ID, to_expr);
    el2_expr = create_expression(el2_reg, REGISTER);
    
    storeArrayElement(program, array->ID, to_expr, el1_expr);
    storeArrayElement(program, array->ID, from_expr, el2_expr);
    
    /* Update indices */
    gen_addi_instruction(program, from_reg, from_reg, 1);
    gen_subi_instruction(program, to_reg, to_reg, 1);
    
    /* Go back to the condition check */
    gen_bt_instruction(program, condition_label, 0);
    
    /* The End */
    assignLabel(program, end_label);
        
}
