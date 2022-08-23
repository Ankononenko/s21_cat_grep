COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
PATTERN="Hello"
FILENAME1="1.txt"
FILENAME2="2.txt"
FILENAME3="3.txt"
FILENAME4="4.txt"
FILENAME5="5.txt"

cat /dev/null > log.txt

for var in -v -c -l -n -i -o -h
do
  for var2 in -v -c -l -n -i -o -h
  do
      for var3 in -v -c -l -n -i -o -h
      do
        for var4 in -v -c -l -n -i -o -h
        do
          for var5 in -v -c -l -n -i -o -h
          do
            for var6 in -v -c -l -n -i -o -h
            do
              if [ $var != $var2 ] && [ $var != $var2 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var != $var5 ] && [ $var != $var6 ] && [ $var2 != $var3 ] && [ $var2 != $var4 ] && [ $var2 != $var5 ] && [ $var2 != $var6 ] && [ $var3 != $var4 ] && [ $var3 != $var5 ] && [ $var3 != $var6 ] && [ $var4 != $var5 ] && [ $var4 != $var6 ] && [ $var5 != $var6 ]
              then
                TEST1="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST1"
                ./s21_grep $TEST1 > s21_grep.txt
                grep $TEST1 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST1" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

                TEST2="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST2"
                ./s21_grep $TEST2 > s21_grep.txt
                grep $TEST2 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST2" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

                TEST3="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST3"
                ./s21_grep $TEST3 > s21_grep.txt
                grep $TEST3 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST3" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

                TEST4="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST4"
                ./s21_grep $TEST4 > s21_grep.txt
                grep $TEST4 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST4" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

                TEST5="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST5"
                ./s21_grep $TEST5 > s21_grep.txt
                grep $TEST5 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST5" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

                TEST6="$var $var2 $var3 $var4 $var5 $var6 $PATTERN $FILENAME1 $FILENAME2 $FILENAME3 $FILENAME4 $FILENAME5"
                echo "$TEST6"
                ./s21_grep $TEST6 > s21_grep.txt
                grep $TEST6 > grep.txt
                DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                  then
                    (( COUNTER_SUCCESS++ ))
                  else
                    echo "$TEST6" >> log.txt
                    (( COUNTER_FAIL++ ))
                fi
                rm s21_grep.txt grep.txt

              fi
            done    
          done
        done
      done
  done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
