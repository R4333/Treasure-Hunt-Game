

template<class T>
class Stack {
	private:
		struct Node {
			T data;
			Node* next;

			Node()
			{
				next = 0;
			}
			
			Node(T val, Node* ptr = 0)
			{
				data = val;
				next = ptr;
			}
		};
		
		Node * top;
		
	public:
		Stack()
		{
			top = 0;
		}
		
		bool isEmpty()
		{
			if(top == 0)
				return true;
			
			return false;
		}
		
		bool push(T val)
		{
			top = new Node(val, top);
			
			return true;
		}
		
		bool pop()
		{
			if(!isEmpty())
			{
				Node * temp = top;
			
				top = top -> next;
				
				delete temp;
				
				return true;
			}
			
			return false;			
		}
		
		bool Top(T& e)
		{
			if(!isEmpty())
			{
				e = top -> data;
				
				return true;
			}
			
			return false;
		}
		
		void print()
		{
			if(!isEmpty())
			{
				Node * temp = top;
			
				while(temp != 0)
				{
					cout << temp -> data << endl;
					temp = temp -> next;
				}
			}
			
			else
				cout << "Stack is EMPTY" << endl;
			
		}
		
		~Stack()
		{
			if(!isEmpty())
			{
				Node * temp = top;
				
				while(top != NULL)
				{
					temp = top;
					top = top -> next;
					delete temp;
				}
			}
		}
};

