/*******************************************************************************
+
+  _bounded_ordered_partition.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  Partition of a finite integer set where each cell is associated with 
+  additional information
+
*******************************************************************************/

#ifndef BOUNDED_ORDERED_PARTITION_H
#define BOUNDED_ORDERED_PARTITION_H

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

template<typename ord_t, class cell_info_t>
class bounded_ordered_partition
{

public:

 typedef ord_t item;

 struct cell_t : public cell_info_t
 {
   ord_t start, index;
  };


private:

 ord_t size, cell_count, elem_count;
 ord_t* elems;
 cell_t* cells;
 
public:

 void clear() { elem_count = 0; cell_count = 0; }

 bounded_ordered_partition(ord_t _size=1)
 { size = _size;
   elems = new ord_t[size];
   cells = new cell_t[size + 1];
   clear();
  }

~bounded_ordered_partition()
 { delete[] elems;
   delete[] cells;
  }

 cell_t& new_cell(const cell_info_t& ci)
 { (cell_info_t&)cells[cell_count] = ci;
   cells[cell_count].start = elem_count;
   cells[cell_count].index = cell_count;
   cell_count++;
   return cells[cell_count - 1];
  }

 cell_t& new_cell()
 { cells[cell_count].start = elem_count;
   cells[cell_count].index = cell_count;
   return cells[cell_count++];
  }

 void copy_cell(const bounded_ordered_partition& bop, int ci)
 { new_cell(bop[ci]);
   for(int ei = bop[ci].start; ei < bop[ci + 1].start; ei++)
   append_to_curr_cell(bop.element(ei));
  }

 void  append_to_curr_cell(ord_t e) { elems[elem_count++] = e; }
 void  finish() { cells[cell_count].start = elem_count; }

 ord_t element(ord_t ei)    const { return elems[ei]; }
 ord_t first_elem(ord_t ci) const { return cells[ci].start; }
 ord_t end_elem(ord_t ci)   const { return cells[ci + 1].start; }

 void print(ostream& o) const
 { for(ord_t c = 0; c < cell_count; c++)
   { o << (cell_info_t&)cells[c] << '\t';
     for(ord_t e = cells[c].start; e < cells[c + 1].start; e++)
       o << elems[e] << ' ';
     o << endl;
    }
  }

 ord_t num_elem()            const { return elem_count; }
 cell_t& operator[](ord_t i) const { return cells[i]; }
 ord_t num_cells()           const { return cell_count; }
 ord_t num_elem(int c)       const { return cells[c+1].start-cells[c].start; }



 void clear_after(ord_t c)
 { cell_count = c;
   elem_count = cells[cell_count].start;
  }

 bool compatible(const bounded_ordered_partition& bop2, 
                 const leda_cmp_base<cell_info_t>& initial_comp) const
 {
   if(num_cells() != bop2.num_cells()) return false;

    for(ord_t c = 0; c < cell_count; c++)
    { if(num_elem(c) != bop2.num_elem(c)) return false;
      if(initial_comp(cells[c], bop2.cells[c]) != 0) return false;
     }

    return true;
   }
    

 bool equivalent(const bounded_ordered_partition& bop2, 
                 const leda_cmp_base<cell_info_t>& initial_comp) const
 {
   if(num_cells() != bop2.num_cells()) return false;

   for(ord_t c = 0; c < cell_count; c++)
   { if(num_elem(c) != bop2.num_elem(c)) return false;
     if(initial_comp(cells[c], bop2.cells[c]) != 0) return false;
    }

   return true;
  }

};


template<class ord_t, typename cell_info_t>
ostream& operator<<(ostream& o, 
                    const bounded_ordered_partition<ord_t,cell_info_t>& bop)
{ bop.print(o);
  return o;
}


LEDA_END_NAMESPACE

#endif
