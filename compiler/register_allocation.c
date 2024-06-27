#include "register_allocation.h"

register_pool* init_register_pool()
{
    /* calloc will init everything to zero or NULL */
    register_pool *rp = (register_pool*)calloc(1, sizeof(register_pool)); 
    return rp;
}


void add_id_to_register(register_pool *rp, IR_register reg, char *id)
{
    rp->registers[reg] = realloc(rp->registers[reg], ++(rp->count[reg]));
    rp->registers[reg][(rp->count[reg])-1] = id;

    //print_register_allocation_ALL(rp);
}


void remove_id_from_register(register_pool *rp, IR_register reg, char *id)
{
    for(int i = 0; i < rp->count[reg]; i++)
    {
        if(strcmp(id, rp->registers[reg][i]) == 0)
        {
            for(int j = i; j < rp->count[reg]-1; j++)
            {
                // shift elements
                rp->registers[reg][j] = rp->registers[reg][j+1]; 
            }

            rp->registers[reg] = realloc(rp->registers[reg], --(rp->count[reg]));
            return;
        }
    }

    printf("Element %s was not found in register %d.\n", id, reg);
}

void remove_register_allocation_ALL(register_pool *rp)
{
    for(int i = t0; i <= t2; i++)
    {
        while(get_register_count(rp, i) != 0)
        {
            char* id = get_id_from_register(rp, i);
            remove_id_from_register(rp, i, id);
        }
    }
    for(int i = t3; i <= t6; i++)
    {
        while(get_register_count(rp, i) != 0)
        {
            char* id = get_id_from_register(rp, i);
            remove_id_from_register(rp, i, id);
        }
    }
}


void print_register_allocation(register_pool *rp, IR_register reg)
{
    if(rp->count[reg] == 0)
        printf("Register %d empty.\n", reg);
    else
    {
        printf("Register %d: ", reg);
        for(int i = 0; i < rp->count[reg]; i++)
        {
            printf("%s ", rp->registers[reg][i]);
        }
        printf("\n");
    }
}

void print_register_allocation_ALL(register_pool *rp)
{
    printf("*** Registers: ***\n");
    for(int i = t0; i <= t2; i++)
    {
        print_register_allocation(rp, i);
    }
    for(int i = t3; i <= t6; i++)
    {
        print_register_allocation(rp, i);
    }
    printf("\n");
}

/* Find ID if it is live in any of register. Return register if found, else return 0. */

IR_register find_ID(register_pool *rp, const char* id)
{
    // For now I work only with t0-t6 registers
    for(int i = t0; i <= t2; i++)
    {
        for(int j = 0; j < rp->count[i]; j++)
        {
            if(strcmp(id, rp->registers[i][j]) == 0)
            {
                return i;
            }
        }
    }
    for(int i = t3; i <= t6; i++)
    {
        for(int j = 0; j < rp->count[i]; j++)
        {
            if(strcmp(id, rp->registers[i][j]) == 0)
            {
                return i;
            }
        }
    }

    return 0;
}

void store_19(register_pool *rp)
{
    // For now I work only with t0-t6 registers
    for(int i = t0; i <= t2; i++)
    {
        for(int j = 0; j < rp->count[i]; j++)
        {
            if(strcmp("19", rp->registers[i][j]) == 0)
            {
                remove_id_from_register(rp, i, "19");
            }
        }
    }
    for(int i = t3; i <= t6; i++)
    {
        for(int j = 0; j < rp->count[i]; j++)
        {
            if(strcmp("19", rp->registers[i][j]) == 0)
            {
                remove_id_from_register(rp, i, "19");
            }
        }
    }
    for(int i = a0; i <= a7; i++)
    {
        for(int j = 0; j < rp->count[i]; j++)
        {
            if(strcmp("19", rp->registers[i][j]) == 0)
            {
                remove_id_from_register(rp, i, "19");
            }
        }
    }

}


IR_register find_empty_register(register_pool *rp)
{
    // For now I work only with t0-t6 registers
    for(int i = t0; i <= t2; i++)
    {
        if(rp->count[i] == 0)
            return i;
    }
    for(int i = t3; i <= t6; i++)
    {
        if(rp->count[i] == 0)
            return i;
    }

    return 0;
}

IR_register find_empty_register_without_REGs(register_pool *rp, IR_register reg1, IR_register reg2)
{
    // For now I work only with t0-t6 registers
    for(int i = t0; i <= t2; i++)
    {
        if(i == reg1) continue;
        if(i == reg2) continue;
        if(rp->count[i] == 0)
            return i;
    }
    for(int i = t3; i <= t6; i++)
    {
        if(i == reg1) continue;
        if(i == reg2) continue;
        if(rp->count[i] == 0)
            return i;
    }

    return 0;
}


IR_register min_count_register(register_pool *rp)
{
    // For now I work only with t0-t6 registers
    int min = rp->count[t0];
    IR_register min_reg = t0;
    for(int i = t1; i <= t2; i++)
    {
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }
    for(int i = t3; i <= t6; i++)
    {
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }

    return min_reg;
}


IR_register min_count_register_without_ID(register_pool *rp, char* id1)
{
    IR_register limit_reg1 = find_ID(rp, id1);

    // For now I work only with t0-t6 registers
    int min = 99; // avoids bug if t0 is forbidden register...
    IR_register min_reg = t0;
    for(int i = t1; i <= t2; i++)
    {
        if(i == limit_reg1) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }
    for(int i = t3; i <= t6; i++)
    {
        if(i == limit_reg1) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }

    return min_reg;
}


IR_register min_count_register_without_IDs(register_pool *rp, char* id1, char* id2)
{
    IR_register limit_reg1 = find_ID(rp, id1);
    IR_register limit_reg2 = find_ID(rp, id2);

    // For now I work only with t0-t6 registers
    int min = 99; // avoids bug if t0 is forbidden register...
    IR_register min_reg = t0;
    for(int i = t1; i <= t2; i++)
    {
        if(i == limit_reg1) continue;
        if(i == limit_reg2) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }
    for(int i = t3; i <= t6; i++)
    {
        if(i == limit_reg1) continue;
        if(i == limit_reg2) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }

    return min_reg;
}

IR_register min_count_register_without_REGs(register_pool *rp, IR_register reg1, IR_register reg2)
{
    // For now I work only with t0-t6 registers
    int min = 99; // avoids bug if t0 is forbidden register...
    IR_register min_reg = t0;
    for(int i = t1; i <= t2; i++)
    {
        if(i == reg1) continue;
        if(i == reg2) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }
    for(int i = t3; i <= t6; i++)
    {
        if(i == reg1) continue;
        if(i == reg2) continue;
        if(rp->count[i] < min)
        {
            min = rp->count[i];
            min_reg = i;
    	}
    }

    return min_reg;
}


int ht_find_id_elsewhere(register_pool *rp, ht* table, const char* id)
{
    ht_entry *temp = get_ht_entry(table, id);

    if(temp == NULL)
    {
        printf("No %s ID (key) in table.\n", id);
        return 0;
    }

    if(temp->AD_count > 1)
    // remove it?
        return 1;
     
    return 0;    
}

int get_register_count(register_pool* rp, IR_register reg)
{
    return rp->count[reg];
}

char* get_id_from_register(register_pool* rp, IR_register reg)
{
    if(rp->count[reg] == 0)
        return NULL;
    else
        return rp->registers[reg][0];
}

IR_register find_empty_argument_register(register_pool *rp)
{
    for(int i = a0; i <= a7; i++)
    {
        if(rp->count[i] == 0)
            return i;
    }

    return 0;
}

IR_register find_non_empty_argument_register(register_pool *rp)
{
    for(int i = a0; i <= a7; i++)
    {
        if(rp->count[i] != 0)
            return i;
    }

    return 0;
}