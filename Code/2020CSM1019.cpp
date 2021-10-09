//Suthram Vinay Kumar & 2020CSM1019

#include <bits/stdc++.h>

using namespace std;

struct line //struct for each line
{
    int tid;    //for Transaction ID
    int sa;     //for sales amount
    string cname;   //customer name
    int cat;    //transaction category
};
line wordtostruct(string str1); //This function is used to format the fetched line to struct variable
bool comparesalamount(const line &a, const line &b);
void runs_greater_M(int B, int M, int runs, int cur_blks, int p);

int Records;

int main()
{
    int R;  //for total records
    ofstream inf("input.txt");  //creating a file name for the synthetic table file
    srand(time(0));   //resetting the time to current time to generate new random numbers
    cout<<"How many records do you want the file to be contained(Enter R): ";
    cin>>R;
    Records = R;
    for(int id = 1 ; id <= R; id++) //this loop will run upto no. of records and allocate values accordingly for the synthetic table
    {
        inf<<id<<" "<<(1+(rand()%60000))<<" ";  //Id value with random sale amount within 60000
        string cname = "";
        char c;
        for(int i = 0; i<3; i++)    //generating 3 random characters and forming as a string
        {
            c = 'a' + (rand()%26);
            cname = cname + c;
        }
        inf<<cname<<" "<<(1+(rand()%1500)); //customer name and random category within 1500
        if(id != R) //a new empty line will insert after every record insertion in the file except at the end
            inf<<endl;
    }
    inf.close();//closing the file pointer
    cout<<"\nThe file is created with " <<R<< " number of random records!"<<endl<<endl;
    int B;  //to store the number of records per block
    cout<<"How many records do you want a disk block contain(Enter B): ";
    cin>>B;
    cout<<"\nPlease wait while the file is partitioning into blocks..."<<endl<<endl;

    ifstream iff("input.txt");  //opening input file which is synthetic table file
    string str; //to store each line of the file
    int fname = 1; //first file name for partition files names
    string ext = to_string(fname) + ".txt"; //storing the .txt file
    string firstd = ext;    //storing the first block name of partition files
    while(!iff.eof())   //loop will go upto end of synthetic table file
    {
        ofstream ichunks(ext);  //creating the file name for partitioned block
        cout<<ext;
        for(int i=0; i<B; i++)  //each block will get sorted with B number of records
        {
            if(!iff.eof())  //if the file pointer is not the end of the synthetic file
            {
                getline(iff, str);  //then fetch the line from file
                ichunks<<str<<endl; //and store it into the block file
            }
            if(iff.eof())   //if it's end of the file then store no more at the end of the last block
            {
                cout<<"No More";
                ichunks<<"No More";
                break;
            }
            if(i == B-1)    //suppose if the B records are stored in one block, then store next block name in the next line
            {
                fname++;
                ext = "";
                ext = to_string(fname) + ".txt";
                ichunks<<ext;
                ichunks.close();
            }
        }
    }
    iff.close();

    int totalblk = (int) (ceil((double)R/ (double)B));  //calculating total blocks created for the file

    cout<<"\nThe file partition is finished with "<<totalblk<<" blocks"<<endl<<endl;

    int M;  //to store the number of blocks per memory
    cout<<"How many disk blocks do you want the main memory to contain(Enter M): ";
    cin>>M;

    cout<<"\nPlease wait while generating initial runs..."<<endl<<endl;

    int trec = B*M; //calculation to get the possible amount of records in the memory to sort it internally
    vector<line> ts;    //this vector is to store all those lines

    string ds = firstd; //to start from the first block
    string str1 = "";   //to store the lines from file
    ifstream df(ds);    //opening the block
    int outf = 1;   //to create filenames for runs
    string op = to_string(outf);
    int runs = 0;   //to store the number of runs
    string ofname = "run"+to_string(runs+1)+"out" + op + ".txt";    //formatting the file name for blocks for each run

    for(int j = 0; j<R; j++)    //this loop will create the runs
    {
        getline(df, str1);  //getting the data from partitioned blocks
        for(int i=1; i<=trec;i++)   //loop will run upto total number of records that can fit into the memory
        {
            //cout<<str1;
            if (str1.find(".txt") == string::npos)  //if the fetched line is not .txt text
            {
                line myline = wordtostruct(str1);  //then convert the string line into struct to store in vector
                ts.push_back(myline);
                if(i != trec)   //once the i == trec no need of inrementing j, it will automatically get incremented as the inner loop will be break
                    j++;
            }
            getline(df, str1);  //then fetch next line
            if ( str1.find(".txt") != string::npos) //and if it contains .txt, then the file pointer will fetch next sequential block
            {
                df.close();
                //cout<<str1;
                df.open(str1);
                if(i != trec)   //as it's not a record, the i shouldn't increment
                    i--;
                continue;
            }
            if (str1.find("No More") != string::npos)   //if the line is no more, then there's nothing more to fetch from the files
            {
                df.close();
                //j++;
                break;
            }
        }
        sort(ts.begin(), ts.end(), comparesalamount);   //this is to sort all the elements that can be fit within the memory

        int i =0;   //this variable is used to track the number of records fetched from the memory
        while(i<ts.size())  //to store the records into block
        {
            ofstream oo(ofname);    //opening the blocks of runs
            for(int j =0; j<B; j++) //upto B records the block will contain
            {
                string s = to_string(ts[i].tid) +" "+ to_string(ts[i].sa) +" "+ (ts[i].cname) +" "+ to_string(ts[i].cat); //converting each record into string
                oo<<s;  //then write the string into the block
                if(j <B-1)  //for no empty line at the end of the block
                    oo<<endl;
                i++;    //after fetching each record the i should be incremented
                if(i==ts.size())    //if the all records are fetched then break
                    break;
            }
            ofname = "run"+to_string(runs+1)+"out" + to_string(++outf) + ".txt";//updating file name for new block of the run
        }
        runs++; //incrementing to next run
        outf = 1;   //and output block of run will start from first
        ofname = "run"+to_string(runs+1)+"out" + to_string(outf) + ".txt";  //updating the block name
        ts.clear(); //after each run clear the memory

    }
    cout<<"\nTotal "<<runs<<" initial runs are generated"<<endl<<endl;

    cout<<"\nExteral merge sort is performing..."<<endl<<endl;


    if(M>runs)  //if the number of runs generated are less than the number of blocks in memory
    {
    vector <vector<line>> sim;  //creating a 2D vector for storing one block of each runs
    string ofnn;    //this variable is store the block name of the run
    vector<line> te;
    for(int rn = 0 ; rn<runs; rn++) //to fetch the first blocks of all runs
    {
        string strs;    //to store the record
        ofnn = "run"+to_string(rn+1)+ "out"+to_string(1)+".txt";    //fetching the block name of each run
        ifstream iop(ofnn); //opening the block
        while(getline(iop, strs))   //fetching all lines from the block
        {
            line myline = wordtostruct(strs);  //converting the fetched record into struct
            te.push_back(myline);  //then push into one vector
        }
        sim.push_back(te);  //after getting all records from fist block of any run, pushing it into the 2D vector
        te.clear(); //clearing the pushed vector to fetch next block freshly
    }

    int lastrec =(R-((((int)ceil(((double)R/(double)B)))-1)*B) );   //calculating the number of records in last block of last run
    int lastblk = ( ((int)ceil(((double)R/(double)B))) - ((sim.size()-1)*M) );  //calculating the number of blocks in last run

    vector<int> indices(sim.size(), 0); //to maintain the indices of each block of the run

    vector <int> rf(runs, 1);   //this is to maintain the number of blocks read from each run

    int ffo = 0;    //for the output blocks file name

    vector<line> tempo; //this is to store the each minimum record of all 2D vector elements


    //cout<<endl<<runs<<" "<<runs<<endl<<endl<<runs;
    int i = 1;  //intializing the number of records

    while(i<=R)
    {
        int minein = 0, minv = INT_MAX; //to store the minimum record index of particular run
        for(int rn = 0; rn < runs; rn++)    //run upto all runs and try finding the minimum value
        {
            if(rn == runs-1 && rf[rn] ==  lastblk && indices[rn] < lastrec && sim[rn][indices[rn]].sa < minv)   //if the index pointing to last run and if records are still available to fetch
            {
                    minv = sim[rn][indices[rn]].sa;
                    minein = rn;
            }
            else if(indices[rn]<B && sim[rn][indices[rn]].sa < minv)    //except the last run, if the records are still there to fetch from the particular block
            {
                minv = sim[rn][indices[rn]].sa;
                minein = rn;
            }

        }

        tempo.push_back(sim[minein][indices[minein]]);  //storing the minimum records into one vector
        indices[minein]++;  //then increment the pointer to next record of the run

        if((tempo.size() == B) || (i == R)) //if we get B number of minimum records or if all the records are fetched, then store the tempo vector records into one block
        {
            string sw = "Sorted_Out"+ to_string(++ffo)+".txt";  //creating the file name of final output blocks
            ofstream osw(sw);   //opening the output block
            for(int i = 0; i<tempo.size(); i++) //storing all records into the block
            {
                osw<<(to_string(tempo[i].tid))<<" "<<(to_string(tempo[i].sa))<<" "<<tempo[i].cname<<" "<<(to_string(tempo[i].cat))<<endl;
            }
            if(i==R)
                osw<<"No More";
            else
                osw<<"Sorted_Out"+ to_string(ffo+1)+".txt";
            tempo.clear();  //then clear the vector
        }

        //the below if condition is to check if the all records are fetched from one block of run, if yes, then fetch new block records
        if(((indices[minein]==B) && (rf[minein] < M) && !((minein == sim.size()-1) && (rf[minein]== lastblk)) ))
        {
            indices[minein] = 0;    //resetting the index value of run
            string swr = "run"+to_string(minein+1) +"out"+ to_string(++rf[minein]) +".txt"; //storing the block name of run
            //cout<<endl<<swr<<endl<<endl;
            ifstream fin(swr);  //opening the block of run
            string fstr;
            vector<line> stmp;  //to store the fetched line
            while(getline(fin, fstr))   //fetching all records from the block
            {
                line myline = wordtostruct(fstr);  //converting the line
                stmp.push_back(myline);    //then storing into one vector
            }
            sim[minein] = stmp; //after fetching all records from block then allocate it to particular run of 2D vector
        }

        i++;    //after fetching each element incrementing the number of records we fethed
    }
    }
    else runs_greater_M(B, M, runs, M, 1);

    cout<<endl<<"Intermediate blocks are generated and a sorted output file is generated with Output.txt"<<endl<<endl;

    string strs= "Sorted_Out"+ to_string(1)+".txt"; //fetching final sorted blocks and merging into one one single Output.txt file
    ifstream opin(strs);
    string ss;
    ofstream outt("Output.txt");    //final sorted output file name
    int i =0;
    while(getline(opin, ss))
    {
        if(ss.find(".txt") != string::npos) //if the line contains .txt then it's next block file name, open that file
        {
            opin.close();
            opin.open(ss);

        }
        else if(ss.find("More")!= string::npos)  //if the line No More, then all the data is fetched
            break;
        else
        {
            outt<<ss;   //if the line has the data, then just store it into Output.txt
            if(++i < R) //to avoid unnecessary empty line at the end of file
                outt<<endl;
        }
    }
}


line wordtostruct(string str1)  //this function converts the fetched record string into struct variable
{
        istringstream sss(str1);    //streaming the word
        string word;
        sss>>word;

        stringstream s(word);
        int idd = 0;
        s>>idd; //fetching id value from the record

        sss>>word;
        stringstream sls(word);
        int saa = 0;    //then sales amount
        sls>>saa;

        sss>>word;
        string cn = word;   //then customer name

        sss>>word;
        stringstream sct(word); //then category
        int catt = 0;
        sct>>catt;

        line myline = {idd, saa, cn, catt}; // then storing all into one struct variable
        return myline;  //then return the line

}

bool comparesalamount(const line &a, const line &b) //this function is helping the in_built sort function with comparing the sales amount condition
{
    return a.sa < b.sa;
}

void runs_greater_M(int B, int M, int runs, int cur_blks, int p)
{

    int prv_run_blk = cur_blks; //taking previous runs blocks
    int mm = M-1;   //to store the number of blocks that can be used for external sorting
    vector <vector<line>> sim(mm, vector<line> (B));   //initializing 2D vector same as in R<M case
    vector<line> tempo(B);  //to store each sorted B records
    vector<line> te;    //to store a block of each run, then store into 2D vector

    int tb = ((int)ceil(((double)Records/(double)B)));  //calculating total blocks
    int rf[mm];         //to store the blocks read from memory
    int lastrunblocks  = runs%mm;   //to store the number of last run blocks
    int  lastblk;
    if(p != 1) lastblk = tb%cur_blks;    //if it's first pass, the number of last blocks are as usual of normal runs
    else lastblk = tb - (runs-1)*cur_blks;  //else it should be based on current pass
    int lastbr = Records-(tb-1)*B;  //storing the last block records

    string strs, ss;
    int rn = 0,rr = B, run = 1, rc = 0; //
    while(rn<runs) //for reading first block from each of runs to be merged
    {
        vector<int> indices(mm, 0);
        vector<int> lb(mm, 0);
        int temp_rc = 0, ffo = 1; rc++; //the run counts will increment and output file name will start from 1

        if(runs < rn+mm)    //if the runs are less than the sum of present runs and main memory blocks
            mm = lastrunblocks; //then change main memory blocks to last run blocks
        for(int i = rn; i < rn+mm; i++) //this loop will run upto sum of present runs and main memory blocks
        {
            string strs;    //for storing the file name
            if(p != 1) strs = "p"+to_string(p-1)+"r"+to_string(i+1)+"o"+to_string(ffo)+".txt"; //if it's not first pass, passed runs
            else strs = "run"+to_string(i+1)+"out"+to_string(ffo)+".txt";//otherwise initial runs will be fetched
            ifstream ipp(strs); //opening the fetched block
			string ss;
            while(getline(ipp, ss)) //fetching the line into ss variable
            {
                if(ss.find(".txt") != string::npos )    //if it contains .txt ,then it means go to next block
                    break;
                else if((ss.find("More") != string::npos))  //if no more line then reset the last blocks run
                {
                    lb[i-rn] = 1;
                    break;
                }
                else    //else just push the elements
                {
                    line myline = wordtostruct(ss);
                    te.push_back(myline);
                }
            }
            sim[i-rn] = te; //for each B blocks, 2D vector will get one 1D vector
            te.clear(); //clearing 1D after saved it
            ipp.close();    //closing the file
			rf[i-rn] = 1;   //resettting the number of blocks read
        }

        int runrec; //total records
        if(runs > rn+mm) runrec = cur_blks*mm*B;    //if last run
        else runrec = (cur_blks*(mm-1)+lastblk)*B - B + lastbr; //otherwise it will get updated, remaning runs asusually

        for(int rec = 1; rec<= runrec; rec++)   //loop will run upto number of records we fetched in runs
        {
            int minv = INT_MAX, minein = 0; //finding the mininum record with index of run
            for(int k = 0; k < mm; k++) //upto memory blocks
            {
                if(lb[k] == 1 && rf[k] ==  lastblk && indices[k] < lastbr && sim[k][indices[k]].sa < minv)  //if the min record is in last
                {
                    minv = sim[k][indices[k]].sa;
                    minein = k;

                }
                else if(indices[k]<B && sim[k][indices[k]].sa < minv) //if min record is in earlier runs
                {
                    minv = sim[k][indices[k]].sa;
                    minein = k;
                }
            }

            tempo[temp_rc]=(sim[minein][indices[minein]]); //pushing the min record into tempo vector after getting it
            temp_rc++;  //the number elements are increased
            indices[minein]++;  //number of records in the particular block of run

            if(temp_rc == B || rec == runrec)   //if we get B sorted records or the number of records we fetched is full
            {
                ofstream opp;   //fetching one output block name
                if(runs < M)    //if runs are less, then print it to out block
                    opp.open("Sorted_Out"+to_string(ffo++)+".txt");
                else    //otherwise next pass block
                    opp.open("p"+to_string(p)+"r"+to_string(rc)+"o"+to_string(ffo++)+".txt");

				int rr = B; //total blocks
                if(temp_rc != B && rec == runrec) rr = lastbr; //if the last run

                for(int ip = 0; ip < rr; ip++)  //storing records into file
                    opp<<tempo[ip].tid<<" "<<tempo[ip].sa<<" "<<tempo[ip].cname<<" "<<tempo[ip].cat<<endl;

                if(runs < M)    //next block name at the end of file
                {
                    if(rn+mm >= runs && rec == runrec) opp<<"No More";
                    else opp<<"Sorted_Out"+to_string(ffo)+".txt";
                }

                else    //if runs are more, then next pass block will get stored into current block
                {
                    if(rn+mm >= runs && rec == runrec) opp<<"No More";
                    else if(ffo <= cur_blks*(M-1)) opp<<"p"+to_string(p)+"r"+to_string(rc)+"o"+to_string(ffo)+".txt";
                    else opp<<"p"+to_string(p)+"r"+to_string(rc+1)+"o"+to_string(1)+".txt";
                }
                opp.close();
                temp_rc = 0;    //resetting
            }

            if(lb[minein] == 0 && indices[minein] == B && rf[minein] < cur_blks)    //for fetching the next block
            {
                ifstream ipp;
                indices[minein] = 0;    //resetting the indices
                string strs;
                if(p == 1) strs = "run"+to_string(minein+run)+"out"+to_string(++rf[minein])+".txt"; //if first pass, run block name
                else strs = "p"+to_string(p-1)+"r"+to_string(minein+run)+"o"+to_string(++rf[minein])+".txt";    //or else pass block name

                ipp.open(strs); //opening the block
                string ss;
                for(int k=0; k<B; k++)  //upto B records.
                {
                    getline(ipp, ss);
                    if(ss=="No More")   //if it's the last block and last record with less than B blocks in it
                    {
                        lb[minein] = 1; //then resetting the last blocks index
                        break;
                    }
                    line myline = wordtostruct(ss); //fetching the line
                    te.push_back(myline);   //push it into 1D
                }
                sim[minein] = te;   //then 1D will be assigned to 2D
                te.clear(); //clear all of 1D
                getline(ipp, ss);   //suppose if the last block has same number of records as other block
                if(ss== "No More")lb[minein] = 1;   //so resetting on that condition
                ipp.close();
            }
        }
        run += mm;  //runs will be incremented
        rn +=mm;

    }

    if(runs>M-1) runs_greater_M(B, M, (int)ceil((double)runs/(double)(M-1)), (M-1)*prv_run_blk, ++p); //until the runs becomes less than the number of blocks in memory

}
