# release
openssl req -x509 -sha256 -days 3650 -newkey rsa:4096 \
  -config `pwd`/build/ca.cnf -keyout `pwd`/build/release/ca.key -out `pwd`/build/release/ca.crt &> /dev/null

openssl x509 -in `pwd`/build/release/ca.crt -text -noout &> /dev/null

echo "66612345678966601234567890666" | openssl cms -EncryptedData_encrypt -aes128 -secretkey 31337DEADBEEF666999ABCDEF31337FF -outform der > `pwd`/build/release/ca128.cms
echo "66612345678966601234567890666" | openssl cms -EncryptedData_encrypt -aes256 -secretkey 31337DEADBEEF666999ABCDEF31337FF31337DEADBEEF666999ABCDEF31337FF -outform der > `pwd`/build/release/ca256.cms
echo "66612345678966601234567890666" | openssl cms -EncryptedData_encrypt -aes256 -secretkey 31337DEADBEEF666999ABCDEF31337FF31337DEADBEEF666999ABCDEF31337FF -rc2-128 -outform der > `pwd`/build/release/ca256rc2.cms
