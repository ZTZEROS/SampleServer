#pragma once

template<typename DatumType>
struct RedBlackNode
{
	RedBlackNode<DatumType>* SuperNodeAddress;
	RedBlackNode<DatumType>* LeftSubNodeAddress;
	RedBlackNode<DatumType>* RightSubNodeAddress;
	bool Speciality;
	DatumType Datum;
	//DatumType* ValueAddress;
};

template<typename DatumType>
class RedBlackTree
{
private:
	RedBlackNode<DatumType>* RootNodeAddress;
	RedBlackNode<DatumType> NilNode; //LeafNode;

public:
	RedBlackTree()
	{
		RootNodeAddress = nullptr;
		memset(&NilNode, 0, sizeof(NilNode));
	};
	~RedBlackTree() {};

	void Insert(DatumType datum)
	{
		RedBlackNode<DatumType>* currentNodeAddress;
		RedBlackNode<DatumType>* isoSuperNodeAddress;

		currentNodeAddress = RootNodeAddress;
		isoSuperNodeAddress = nullptr;

		if (currentNodeAddress->LeftSubNodeAddress != nullptr || currentNodeAddress->RightSubNodeAddress != nullptr)
		{
			while (1)
			{
				if (datum < currentNodeAddress->Datum)
				{
					if (currentNodeAddress->LeftSubNodeAddress == &NilNode)
					{
						currentNodeAddress->LeftSubNodeAddress = new RedBlackNode<DatumType>;

						currentNodeAddress->LeftSubNodeAddress->SuperNodeAddress = currentNodeAddress;
						currentNodeAddress->LeftSubNodeAddress->LeftSubNodeAddress = &NilNode;
						currentNodeAddress->LeftSubNodeAddress->RightSubNodeAddress = &NilNode;
						currentNodeAddress->LeftSubNodeAddress->Speciality = 1;
						currentNodeAddress->LeftSubNodeAddress->Datum = datum;

						if (currentNodeAddress->SuperNodeAddress)
						{
							if (currentNodeAddress == currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress) isoSuperNodeAddress = currentNodeAddress->SuperNodeAddress->RightSubNodeAddress;
							else isoSuperNodeAddress = currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress;

							if (isoSuperNodeAddress->Speciality) ColorPromotion(currentNodeAddress->LeftSubNodeAddress);
							else
							{
								currentNodeAddress = currentNodeAddress->LeftSubNodeAddress; //not smart

								RotateRight(currentNodeAddress);
							}
						}

						break;
					}
					
					currentNodeAddress = currentNodeAddress->LeftSubNodeAddress;
				}
				else
				{
					if (currentNodeAddress->RightSubNodeAddress == &NilNode)
					{
						currentNodeAddress->RightSubNodeAddress = new RedBlackNode<DatumType>;

						currentNodeAddress->RightSubNodeAddress->SuperNodeAddress = currentNodeAddress;
						currentNodeAddress->RightSubNodeAddress->LeftSubNodeAddress = &NilNode;
						currentNodeAddress->RightSubNodeAddress->RightSubNodeAddress = &NilNode;
						currentNodeAddress->RightSubNodeAddress->Speciality = 1;
						currentNodeAddress->RightSubNodeAddress->Datum = datum;
						
						if (currentNodeAddress->SuperNodeAddress)
						{
							if (currentNodeAddress == currentNodeAddress->SuperNodeAddress->RightSubNodeAddress) isoSuperNodeAddress = currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress;
							else isoSuperNodeAddress = currentNodeAddress->SuperNodeAddress->RightSubNodeAddress;

							if (isoSuperNodeAddress->Speciality) ColorPromotion(currentNodeAddress->RightSubNodeAddress);
							else
							{
								currentNodeAddress = currentNodeAddress->RightSubNodeAddress;

								RotateLeft(currentNodeAddress);
								RotateRight(currentNodeAddress);
							}
						}

						break;
					}
					
					currentNodeAddress = currentNodeAddress->RightSubNodeAddress;
				}
			}
		}
		else
		{
			currentNodeAddress = new RedBlackNode<DatumType>;

			currentNodeAddress->SuperNodeAddress = nullptr;
			currentNodeAddress->LeftSubNodeAddress = &NilNode;
			currentNodeAddress->RightSubNodeAddress = &NilNode;
			currentNodeAddress->Speciality = 0;
			currentNodeAddress->Datum = datum;
		}
	}

	void Search(DatumType datum)
	{
		RedBlackNode<DatumType>* currentNodeAddress;

		currentNodeAddress = RootNodeAddress;

		while (currentNodeAddress != &NilNode)
		{
			if (datum == currentNodeAddress->Datum) break;

			if (datum < currentNodeAddress->Datum) currentNodeAddress = currentNodeAddress->LeftSubNodeAddress;
			else currentNodeAddress = currentNodeAddress->RightSubNodeAddress;
		}

		return currentNodeAddress;
	}

	template<typename DatumType>
	void ColorPromotion(RedBlackNode<DatumType>* currentNodeAddress)
	{
		RedBlackNode<DatumType>* superNodeAddress;
		RedBlackNode<DatumType>* isoSuperNodeAddress;

		superNodeAddress = currentNodeAddress->SuperNodeAddress;

		if (superNodeAddress == superNodeAddress->SuperNodeAddress->LeftSubNodeAddress) isoSuperNodeAddress = superNodeAddress->SuperNodeAddress->RightSubNodeAddress;
		else isoSuperNodeAddress = superNodeAddress->SuperNodeAddress->LeftSubNodeAddress;

		if (currentNodeAddress->Speciality && superNodeAddress->Speciality)
		{
			superNodeAddress->Speciality = 0;
			isoSuperNodeAddress->Speciality = 0;
			superNodeAddress->SuperNodeAddress->Speciality = 1;

			//if(superNodeAddress != &RootNode) ColorPromotion(superNodeAddress); //bug
		}
	}

	template<typename DatumType>
	void RotateLeft(RedBlackNode<DatumType>* currentNodeAddress)
	{
		RedBlackNode<DatumType>* superNodeAddress;
		RedBlackNode<DatumType>* leftSubNodeAddress;

		superNodeAddress = currentNodeAddress->SuperNodeAddress;
		leftSubNodeAddress = currentNodeAddress->LeftSubNodeAddress;

		superNodeAddress->RightSubNodeAddress = leftSubNodeAddress;
		leftSubNodeAddress->SuperNodeAddress = superNodeAddress;

		currentNodeAddress->LeftSubNodeAddress = superNodeAddress;
		currentNodeAddress->SuperNodeAddress = superNodeAddress->SuperNodeAddress;
		superNodeAddress->SuperNodeAddress = currentNodeAddress;

		if (currentNodeAddress->SuperNodeAddress)
		{
			//if (superNodeAddresss == currentNodeAddress->SuperNodeAddress->RightSubNodeAddress) currentNodeAddress->SuperNodeAddress->RightSubNodeAddress = currentNodeAddress;
			//else currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress = currentNodeAddress;
		}
		else RootNodeAddress = currentNodeAddress;

		//supernode <-> rightSubNode
	}

	template<typename DatumType>
	void RotateRight(RedBlackNode<DatumType>* currentNodeAddress)
	{
		RedBlackNode<DatumType>* superNodeAddress;
		RedBlackNode<DatumType>* rightSubNodeAddress;

		superNodeAddress = currentNodeAddress->SuperNodeAddress;
		rightSubNodeAddress = currentNodeAddress->RightSubNodeAddress;

		superNodeAddress->LeftSubNodeAddress = rightSubNodeAddress;
		rightSubNodeAddress->SuperNodeAddress = superNodeAddress;

		currentNodeAddress->RightSubNodeAddress = superNodeAddress;
		currentNodeAddress->SuperNodeAddress = superNodeAddress->SuperNodeAddress;
		superNodeAddress->SuperNodeAddress = currentNodeAddress;

		if (currentNodeAddress->SuperNodeAddress)
		{
			if (superNodeAddress == currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress) currentNodeAddress->SuperNodeAddress->LeftSubNodeAddress = currentNodeAddress;
			else currentNodeAddress->SuperNodeAddress->RightSubNodeAddress = currentNodeAddress;
		}
		else RootNodeAddress = currentNodeAddress;

		//left subnode <-> supernode
	}

	void ShowTree()
	{

	}

	void Traverse()
	{

	}

	void Delete(DatumType datum)
	{
		RedBlackNode<DatumType>* currentNodeAddress;
		
		RedBlackNode<DatumType>* superNodeAddress;
		RedBlackNode<DatumType>* isoSuperNodeAddress;
		RedBlackNode<DatumType>* leftSubNodeAddress;
		RedBlackNode<DatumType>* rightSubNodeAddress;

		currentNodeAddress = Search(datum);

		if (currentNodeAddress != RootNodeAddress)
		{
			superNodeAddress = currentNodeAddress->SuperNodeAddress;

			if (currentNodeAddress == superNodeAddress->LeftSubNodeAddress) isoSuperNodeAddress = superNodeAddress->RightSubNodeAddress;
			else isoSuperNodeAddress = superNodeAddress->LeftSubNodeAddress;
		}

		leftSubNodeAddress = currentNodeAddress->LeftSubNodeAddress;
		rightSubNodeAddress = currentNodeAddress->RightSubNodeAddress;

		delete currentNodeAddress;

		if (leftSubNodeAddress != &NilNode)
		{
			currentNodeAddress = leftSubNodeAddress;

			while (currentNodeAddress->RightSubNodeAddress != &NilNode)
			{
				currentNodeAddress = currentNodeAddress->RightSubNodeAddress;
			}
		}
		else
		{
			if (rightSubNodeAddress != &NilNode)
			{
				rightSubNodeAddress->SuperNodeAddress = superNodeAddress;

				if (isoSuperNodeAddress == superNodeAddress->LeftSubNodeAddress) superNodeAddress->RightSubNodeAddress = rightSubNodeAddress;
				else superNodeAddress->LeftSubNodeAddress = rightSubNodeAddress;
			}
		}

		//node swap code
		{

		}

		if (currentNodeAddress == &NilNode) return;
		
		if (currentNodeAddress->Speciality)
		{

		}
		else
		{
			if (1)
			{

			}
		}
		
		
		
		
		
	}
};

/*
노드가 최초에 삽입될 때는 레드 노드이다
Red - Red : 트리 가지 길이가 늘어난 상황
Black - Red : 트리의 빈 부분이 채워진 상황
Red - Black : 

좌회전, 우회전, 대기 3가지 경우가 있다
색상반전
*/