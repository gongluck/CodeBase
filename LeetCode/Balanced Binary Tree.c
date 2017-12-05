/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */

int depth(struct TreeNode* root)
{
    if(root == NULL)
        return 0;
    int depthleft = depth(root->left);
    int depthright = depth(root->right);
    return (depthleft>depthright?depthleft:depthright)+1;
}

bool isBalanced(struct TreeNode* root) 
{
    if(root == NULL)
        return true;
    if(abs(depth(root->left) - depth(root->right)) <= 1 && isBalanced(root->left) && isBalanced(root->right))
        return true;
    else
        return false;
}