import time
import os
import shutil
import zlib
import boto3
#import uuid

def s3_connection_bucket(bucket):
    s3 = boto3.resource(
        service_name="s3",
        region_name="ap-northeast-2", # 자신이 설정한 bucket region
        aws_access_key_id="your_key_id",
        aws_secret_access_key="your_access_key",
    )
    buck = s3.Bucket(bucket)
    return buck

def bucket_put_object(bucket, filepath, access_key):
    bucket.upload_file(filepath, access_key)

def bucket_get_dir(bucket, dirpath, prefix):
    for obj in bucket.objects.filter(Prefix=prefix):
        bucket.download_file(obj.key, '/tmp/'+obj.key)
'''
def parse_directory(directory):
    size = 0
    for root, dirs, files in os.walk(directory):
        for file in files:
            size += os.path.getsize(os.path.join(root, file))
    return size
'''
def main(args):
    startTime = time.time()

    zip_dir = args.get("dir", "dir_name")
    dirpath = '/tmp/'+zip_dir #'/tmp/{}-{}'.format(zip_dir, uuid.uuid4()) 
    upload_key = zip_dir+".zip"

    bucket = s3_connection_bucket('bucket')

    if not os.path.exists(dirpath):
        os.makedirs(dirpath)

    bucket_get_dir(bucket, dirpath, zip_dir)
    #size = parse_directory(download_path)

    shutil.make_archive(dirpath, 'zip', root_dir=dirpath)

    bucket_put_object(bucket, dirpath+'.zip', upload_key)

    t6 = time.time()

    return {'startTime': startTime, 'functionTime': t6-startTime}
