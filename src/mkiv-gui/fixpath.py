'''
Created on 10 Oct 2013

@author: Liam Deacon
'''
import platform,os

# Fix for escape characters in Windows-based paths
# also removes invalid characters
def fix_path(file_path):
    if platform.system() is 'Windows':
  
        file_path = os.path.abspath(file_path)
        fix_list = {'\a':'\\a', '\b':'\\b',
                    '\f':'\\f', '\n':'\\n',
                    '\r':'\\r', '\t':'\\t',
                    '\v':'\\v', '\\\\':'\\'}
        for fix in fix_list:
            file_path = file_path.replace(fix, fix_list[fix])
    
        for fix in fix_list:
            file_path = file_path.replace(fix, fix_list[fix])
    
    return "".join(x for x in file_path if x.isalnum() or x in ':\\/-_.')