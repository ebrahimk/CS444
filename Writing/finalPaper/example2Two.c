struct list_head{                       //list_head structure
        struct list_node* next;
        struct list_node* prev;
        };

struct some_data{
        int value;
        struct list_head* lst_head;     //embed the list_head structure 
        }
