#include "comm.hpp"

int main()
{
    FileOper fo_write(PATH,FILENAME);
    fo_write.OpenForWrite();
    fo_write.Write();
    fo_write.Close();
}