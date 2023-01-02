import boto3
import time
import os

def s3_connection():
    try:
        s3 = boto3.client(
            service_name="s3",
            region_name="ap-northeast-2", # 자신이 설정한 bucket region
            aws_access_key_id="your_key_id",
            aws_secret_access_key="your_access_key",
        )
    except Exception as e:
        print(e)
    else:
        #print("s3 bucket connected!")
        return s3

def s3_put_object(s3, bucket, filepath, access_key):
    """
    s3 bucket에 지정 파일 업로드
    :param s3: 연결된 s3 객체(boto3 client)
    :param bucket: 버킷명
    :param filepath: 파일 위치
    :param access_key: 저장 파일명
    :return: 성공 시 True, 실패 시 False 반환
    """
    try:
        s3.upload_file(
            Filename=filepath,
            Bucket=bucket,
            Key=access_key,
            ExtraArgs={"ContentType": "image/jpg", "ACL": "public-read"},
        )
    except Exception as e:
        return False
    return True

def s3_get_object(s3, bucket, filepath, accesskey):
    try:
        s3.download_file(
                Filename=filepath,
                Bucket=bucket,
                Key=accesskey
        )
    except Exception as e:
        return False
    return True


def main(args):
    startTime = time.time()
    s3 = s3_connection()
    res = "Failed"
    if s3_get_object(s3, 'bucket_name', '/your/machine/path', 'file_name'):
        if s3_put_object(s3, 'bucket_name', '/your/machine/path', 'new_file_name'):
            res = "Clear"
        else:
            res = "Only get"
    t4 = time.time()
    return {"startTime": startTime, "functionTime": t4-startTime, "Result": res}
